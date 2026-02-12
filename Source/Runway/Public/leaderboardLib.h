#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "leaderboardLib.generated.h"

UCLASS(Blueprintable)
class RUNWAY_API UleaderboardLib : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UleaderboardLib();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool SubmitPlayerMapFinishTime(float finishTime, const FString& mapId, const FString& PlayerId);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString EncodeStringToBase64(const FString& String);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString decodeBase64String(const FString& String);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static TArray<uint8> ConvertStringToByte(const FString& String);
    
};

