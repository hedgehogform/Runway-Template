#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "encryptionLib.generated.h"

UCLASS(Blueprintable)
class RUNWAY_API UencryptionLib : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UencryptionLib();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString EncryptStringAES(const FString& Data, const FString& Key);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString EncryptBytesAES(TArray<uint8> inBytes, const FString& Key);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString ByteToString(uint8 inByte);
    
};

