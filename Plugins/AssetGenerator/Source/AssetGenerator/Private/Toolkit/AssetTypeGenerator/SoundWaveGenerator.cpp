#include "Toolkit/AssetTypeGenerator/SoundWaveGenerator.h"
#include "Dom/JsonObject.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundClass.h"
#include "Toolkit/ObjectHierarchySerializer.h"
#include "Toolkit/AssetTypes/AssetHelper.h"
#include "AudioDevice.h"

void USoundWaveGenerator::CreateAssetPackage()
{
    UPackage *NewPackage = CreatePackage(
#if ENGINE_MINOR_VERSION < 26
        nullptr,
#endif
        *GetPackageName().ToString());
    USoundWave *NewSoundWave = NewObject<USoundWave>(NewPackage, GetAssetName(), RF_Public | RF_Standalone);
    SetPackageAndAsset(NewPackage, NewSoundWave);

    PopulateSoundWaveWithData(NewSoundWave);
}

void USoundWaveGenerator::OnExistingPackageLoaded()
{
    USoundWave *ExistingSoundWave = GetAsset<USoundWave>();

    // Ensure SoundClassObject is set - if null (reference failed), use engine default
    if (!ExistingSoundWave->SoundClassObject)
    {
        USoundClass *DefaultSoundClass = LoadObject<USoundClass>(nullptr, TEXT("/Engine/EngineSounds/Master.Master"));
        if (DefaultSoundClass)
        {
            ExistingSoundWave->SoundClassObject = DefaultSoundClass;

            // Call PostLoad() to let engine cache SoundClassProperties from the new SoundClassObject
            ExistingSoundWave->PostLoad();

            ExistingSoundWave->MarkPackageDirty();
            UE_LOG(LogAssetGenerator, Warning, TEXT("Loaded SoundWave %s had NULL SoundClassObject - set to engine default and re-initialized"),
                   *ExistingSoundWave->GetPathName());
        }
        else
        {
            UE_LOG(LogAssetGenerator, Error, TEXT("Loaded SoundWave %s has NULL SoundClassObject and could not load engine default!"),
                   *ExistingSoundWave->GetPathName());
        }
    }

    if (!IsSoundWaveUpToDate(ExistingSoundWave))
    {
        UE_LOG(LogAssetGenerator, Display, TEXT("SoundWave %s is not up to date, regenerating data"), *ExistingSoundWave->GetPathName());

        PopulateSoundWaveWithData(ExistingSoundWave);
    }
}

void USoundWaveGenerator::PopulateSoundWaveWithData(USoundWave *SoundWave)
{
    const TSharedPtr<FJsonObject> AssetData = GetAssetData();

    // First, deserialize the JSON properties
    const TSharedPtr<FJsonObject> AssetObjectData = AssetData->GetObjectField(TEXT("AssetObjectData"));
    GetObjectSerializer()->DeserializeObjectProperties(AssetObjectData.ToSharedRef(), SoundWave);

    // Ensure SoundClassObject is set (required to prevent crashes)
    // If JSON had a valid reference, it will already be set by DeserializeObjectProperties
    if (!SoundWave->SoundClassObject)
    {
        // Use engine default SoundClass as fallback
        USoundClass *DefaultSoundClass = FindObject<USoundClass>(nullptr, TEXT("/Engine/EngineSounds/Master.Master"));
        if (!DefaultSoundClass)
        {
            DefaultSoundClass = LoadObject<USoundClass>(nullptr, TEXT("/Engine/EngineSounds/Master.Master"));
        }

        if (DefaultSoundClass)
        {
            SoundWave->SoundClassObject = DefaultSoundClass;
            UE_LOG(LogAssetGenerator, Warning, TEXT("Set default SoundClass %s for SoundWave %s (failed to resolve SoundClass from JSON)"), *DefaultSoundClass->GetPathName(), *SoundWave->GetPathName());
        }
        else
        {
            UE_LOG(LogAssetGenerator, Error, TEXT("Could not find default SoundClass for SoundWave %s - audio will crash on playback!"), *SoundWave->GetPathName());
            return; // Abort - can't create valid SoundWave without SoundClass
        }
    }
    else
    {
        UE_LOG(LogAssetGenerator, Display, TEXT("SoundWave %s using SoundClass %s from JSON"), *SoundWave->GetPathName(), *SoundWave->SoundClassObject->GetPathName());
    }

    // Try to load audio file - check for both .wav and .ogg
    FString AudioFilename = GetAdditionalDumpFilePath(TEXT(""), TEXT("wav"));
    if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*AudioFilename))
    {
        AudioFilename = GetAdditionalDumpFilePath(TEXT(""), TEXT("ogg"));
        if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*AudioFilename))
        {
            UE_LOG(LogAssetGenerator, Warning, TEXT("No audio file found for SoundWave %s (tried .wav and .ogg)"), *SoundWave->GetPathName());
            return;
        }
    }

    // Load the audio file
    TArray<uint8> LoadedAudioData;
    if (!FFileHelper::LoadFileToArray(LoadedAudioData, *AudioFilename))
    {
        UE_LOG(LogAssetGenerator, Error, TEXT("Failed to load audio file %s for SoundWave %s"), *AudioFilename, *SoundWave->GetPathName());
        return;
    }

    if (LoadedAudioData.Num() == 0)
    {
        UE_LOG(LogAssetGenerator, Error, TEXT("Audio file %s is empty for SoundWave %s"), *AudioFilename, *SoundWave->GetPathName());
        return;
    }

    UE_LOG(LogAssetGenerator, Display, TEXT("Loaded audio file %s (%d bytes) for SoundWave %s"), *AudioFilename, LoadedAudioData.Num(), *SoundWave->GetPathName());

    // Determine file extension
    FString Extension = FPaths::GetExtension(AudioFilename).ToLower();

    // For WAV files, we need to parse and load as raw PCM
    if (Extension == TEXT("wav"))
    {
        // Skip the WAV header (44 bytes for standard WAV with PCM)
        // WAV format: RIFF header (12 bytes) + fmt chunk (24 bytes) + data chunk header (8 bytes) = 44 bytes minimum
        int32 HeaderSize = 44;

        // Basic WAV validation
        if (LoadedAudioData.Num() < HeaderSize ||
            LoadedAudioData[0] != 'R' || LoadedAudioData[1] != 'I' ||
            LoadedAudioData[2] != 'F' || LoadedAudioData[3] != 'F')
        {
            UE_LOG(LogAssetGenerator, Error, TEXT("Invalid WAV file format for %s"), *AudioFilename);
            return;
        }

        // Find the data chunk (search for "data" marker)
        int32 DataChunkOffset = -1;
        for (int32 i = 12; i < LoadedAudioData.Num() - 8; i++)
        {
            if (LoadedAudioData[i] == 'd' && LoadedAudioData[i + 1] == 'a' &&
                LoadedAudioData[i + 2] == 't' && LoadedAudioData[i + 3] == 'a')
            {
                DataChunkOffset = i + 8; // Skip "data" and 4-byte size field
                break;
            }
        }

        if (DataChunkOffset < 0)
        {
            UE_LOG(LogAssetGenerator, Error, TEXT("Could not find data chunk in WAV file %s"), *AudioFilename);
            return;
        }

        // Extract PCM data
        TArray<uint8> PCMData;
        PCMData.Append(&LoadedAudioData[DataChunkOffset], LoadedAudioData.Num() - DataChunkOffset);

        // Set the raw data for cooking
        SoundWave->RawData.Lock(LOCK_READ_WRITE);
        void *LockedData = SoundWave->RawData.Realloc(PCMData.Num());
        FMemory::Memcpy(LockedData, PCMData.GetData(), PCMData.Num());
        SoundWave->RawData.Unlock();

        // Calculate Duration based on PCM data size, sample rate, channels, and bit depth
        // Assuming 16-bit PCM (2 bytes per sample)
        int32 BytesPerSample = 2;
        int32 NumChannels = *SoundWave->GetClass()->FindPropertyByName(FName("NumChannels"))->ContainerPtrToValuePtr<int32>(SoundWave);
        int32 SampleRate = *SoundWave->GetClass()->FindPropertyByName(FName("SampleRate"))->ContainerPtrToValuePtr<int32>(SoundWave);

        if (NumChannels > 0 && SampleRate > 0)
        {
            int32 TotalSamples = PCMData.Num() / BytesPerSample / NumChannels;
            SoundWave->Duration = (float)TotalSamples / (float)SampleRate;

            UE_LOG(LogAssetGenerator, Display, TEXT("SoundWave %s: %d samples, %d Hz, %d channels, %.2f seconds"),
                   *SoundWave->GetPathName(), TotalSamples, SampleRate, NumChannels, SoundWave->Duration);
        }

        UE_LOG(LogAssetGenerator, Display, TEXT("Loaded %d bytes of PCM data for SoundWave %s"), PCMData.Num(), *SoundWave->GetPathName());
    }
    else if (Extension == TEXT("ogg"))
    {
        // For OGG Vorbis, store the complete file as compressed data
        SoundWave->RawData.Lock(LOCK_READ_WRITE);
        void *LockedData = SoundWave->RawData.Realloc(LoadedAudioData.Num());
        FMemory::Memcpy(LockedData, LoadedAudioData.GetData(), LoadedAudioData.Num());
        SoundWave->RawData.Unlock();

        UE_LOG(LogAssetGenerator, Display, TEXT("Loaded %d bytes of OGG data for SoundWave %s"), LoadedAudioData.Num(), *SoundWave->GetPathName());
    }

    // Invalidate compressed data to force recompression
    SoundWave->InvalidateCompressedData();

    // Call PostLoad() to initialize cached pointers like SoundClassProperties
    // This follows the pattern from MaterialParameterCollectionGenerator, FontGenerator, etc.
    // The engine's PostLoad() will automatically cache SoundClassProperties from SoundClassObject
    SoundWave->PostLoad();

    // Mark as modified
    SoundWave->MarkPackageDirty();
}

bool USoundWaveGenerator::IsSoundWaveUpToDate(USoundWave *SoundWave) const
{
    const TSharedPtr<FJsonObject> AssetData = GetAssetData();

    // Check if we have the audio payload hash from the dumper
    if (AssetData->HasField(TEXT("AudioPayloadHash")))
    {
        const FString AudioPayloadHash = AssetData->GetStringField(TEXT("AudioPayloadHash"));

        // Get existing data hash
        SoundWave->RawData.Lock(LOCK_READ_ONLY);
        const int32 RawDataSize = SoundWave->RawData.GetBulkDataSize();

        if (RawDataSize > 0)
        {
            TArray<uint8> ExistingData;
            ExistingData.SetNum(RawDataSize);
            const void *BulkData = SoundWave->RawData.LockReadOnly();
            FMemory::Memcpy(ExistingData.GetData(), BulkData, RawDataSize);
            SoundWave->RawData.Unlock();
            SoundWave->RawData.Unlock();

            const FString ExistingDataHash = FAssetHelper::ComputePayloadHash(ExistingData);
            if (ExistingDataHash != AudioPayloadHash)
            {
                return false;
            }
        }
        else
        {
            SoundWave->RawData.Unlock();
            return false; // No data, needs regeneration
        }
    }
    else
    {
        // No hash available, check if we have any audio data
        SoundWave->RawData.Lock(LOCK_READ_ONLY);
        int32 RawDataSize = SoundWave->RawData.GetBulkDataSize();
        SoundWave->RawData.Unlock();

        if (RawDataSize == 0)
        {
            return false; // No data, needs regeneration
        }
    }

    // Check if JSON properties match
    const TSharedPtr<FJsonObject> AssetObjectData = AssetData->GetObjectField(TEXT("AssetObjectData"));
    if (!GetObjectSerializer()->AreObjectPropertiesUpToDate(AssetObjectData.ToSharedRef(), SoundWave))
    {
        return false;
    }

    return true;
}

FName USoundWaveGenerator::GetAssetClass()
{
    return TEXT("SoundWave");
}
