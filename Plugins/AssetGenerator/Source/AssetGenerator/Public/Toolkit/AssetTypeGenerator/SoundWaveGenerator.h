#pragma once
#include "CoreMinimal.h"
#include "Toolkit/AssetGeneration/AssetTypeGenerator.h"
#include "SoundWaveGenerator.generated.h"

UCLASS(MinimalAPI)
class USoundWaveGenerator : public UAssetTypeGenerator
{
    GENERATED_BODY()
protected:
    virtual void CreateAssetPackage() override;
    virtual void OnExistingPackageLoaded() override;
    void PopulateSoundWaveWithData(class USoundWave *SoundWave);
    bool IsSoundWaveUpToDate(class USoundWave *SoundWave) const;

public:
    virtual FName GetAssetClass() override;
};
