#include "Toolkit/AssetTypes/SoundWaveAssetSerializer.h"
#include "Toolkit/AssetDumping/SerializationContext.h"
#include "Toolkit/AssetDumping/AssetTypeSerializerMacros.h"
#include "Toolkit/ObjectHierarchySerializer.h"
#include "Sound/SoundWave.h"
#include "Toolkit/PropertySerializer.h"
#include "Toolkit/AssetTypes/AssetHelper.h"
#include "AudioDevice.h"
#include "AssetDumperModule.h"

void USoundWaveAssetSerializer::SerializeAsset(TSharedRef<FSerializationContext> Context) const
{
    BEGIN_ASSET_SERIALIZATION(USoundWave)

    // Get the raw audio data from the SoundWave
    TArray<uint8> RawAudioData;

    // Access the raw PCM data
    Asset->RawData.Lock(LOCK_READ_ONLY);
    const int32 BulkDataSize = Asset->RawData.GetBulkDataSize();

    if (BulkDataSize > 0)
    {
        RawAudioData.SetNum(BulkDataSize);
        const void *BulkData = Asset->RawData.LockReadOnly();
        FMemory::Memcpy(RawAudioData.GetData(), BulkData, BulkDataSize);
        Asset->RawData.Unlock();
    }
    Asset->RawData.Unlock();

    if (RawAudioData.Num() == 0)
    {
        UE_LOG(LogAssetDumper, Warning, TEXT("SoundWave %s has no raw audio data to export"), *Asset->GetPathName());
    }
    else
    {
        // Export as WAV file
        // WAV file format: RIFF header + fmt chunk + data chunk

        // Get audio properties from reflection since they're protected
        // We'll use reasonable defaults if we can't access them
        int32 NumChannels = 2;    // Default to stereo
        int32 SampleRate = 44100; // Default to 44.1kHz

        // Try to get actual values via property system
        if (FProperty *NumChannelsProperty = Asset->GetClass()->FindPropertyByName(TEXT("NumChannels")))
        {
            const void *PropertyValue = NumChannelsProperty->ContainerPtrToValuePtr<void>(Asset);
            NumChannels = *static_cast<const int32 *>(PropertyValue);
        }
        if (FProperty *SampleRateProperty = Asset->GetClass()->FindPropertyByName(TEXT("SampleRate")))
        {
            const void *PropertyValue = SampleRateProperty->ContainerPtrToValuePtr<void>(Asset);
            SampleRate = *static_cast<const int32 *>(PropertyValue);
        }

        const int32 BitsPerSample = 16; // Assuming 16-bit PCM
        const int32 ByteRate = SampleRate * NumChannels * (BitsPerSample / 8);
        const int32 BlockAlign = NumChannels * (BitsPerSample / 8);
        const int32 DataSize = RawAudioData.Num();
        const int32 ChunkSize = 36 + DataSize;

        TArray<uint8> WavData;
        WavData.Reserve(44 + DataSize);

        // RIFF header
        WavData.Append((uint8 *)"RIFF", 4);
        WavData.Add((ChunkSize >> 0) & 0xFF);
        WavData.Add((ChunkSize >> 8) & 0xFF);
        WavData.Add((ChunkSize >> 16) & 0xFF);
        WavData.Add((ChunkSize >> 24) & 0xFF);
        WavData.Append((uint8 *)"WAVE", 4);

        // fmt chunk
        WavData.Append((uint8 *)"fmt ", 4);
        WavData.Add(16);
        WavData.Add(0);
        WavData.Add(0);
        WavData.Add(0); // Subchunk1Size = 16
        WavData.Add(1);
        WavData.Add(0); // AudioFormat = 1 (PCM)
        WavData.Add(NumChannels & 0xFF);
        WavData.Add((NumChannels >> 8) & 0xFF);
        WavData.Add((SampleRate >> 0) & 0xFF);
        WavData.Add((SampleRate >> 8) & 0xFF);
        WavData.Add((SampleRate >> 16) & 0xFF);
        WavData.Add((SampleRate >> 24) & 0xFF);
        WavData.Add((ByteRate >> 0) & 0xFF);
        WavData.Add((ByteRate >> 8) & 0xFF);
        WavData.Add((ByteRate >> 16) & 0xFF);
        WavData.Add((ByteRate >> 24) & 0xFF);
        WavData.Add(BlockAlign & 0xFF);
        WavData.Add((BlockAlign >> 8) & 0xFF);
        WavData.Add(BitsPerSample & 0xFF);
        WavData.Add((BitsPerSample >> 8) & 0xFF);

        // data chunk
        WavData.Append((uint8 *)"data", 4);
        WavData.Add((DataSize >> 0) & 0xFF);
        WavData.Add((DataSize >> 8) & 0xFF);
        WavData.Add((DataSize >> 16) & 0xFF);
        WavData.Add((DataSize >> 24) & 0xFF);
        WavData.Append(RawAudioData);

        // Record file hash
        Data->SetStringField(TEXT("AudioPayloadHash"), FAssetHelper::ComputePayloadHash(WavData));

        // Save as WAV file
        const FString ResultWavFilename = Context->GetDumpFilePath(TEXT(""), TEXT("wav"));
        if (FFileHelper::SaveArrayToFile(WavData, *ResultWavFilename))
        {
            UE_LOG(LogAssetDumper, Display, TEXT("Exported SoundWave %s to %s (%d bytes)"),
                   *Asset->GetPathName(), *ResultWavFilename, WavData.Num());
        }
        else
        {
            UE_LOG(LogAssetDumper, Error, TEXT("Failed to save audio file %s"), *ResultWavFilename);
        }
    }

    SERIALIZE_ASSET_OBJECT
    END_ASSET_SERIALIZATION
}

FName USoundWaveAssetSerializer::GetAssetClass() const
{
    return USoundWave::StaticClass()->GetFName();
}
