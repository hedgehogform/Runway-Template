#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "customVariableHandling.generated.h"

class UObject;

UCLASS(Blueprintable)
class RUNWAY_API UcustomVariableHandling : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UcustomVariableHandling();

    UFUNCTION(BlueprintCallable)
    static bool SetVectorByName(UObject* Target, FName VarName, FVector NewValue);
    
    UFUNCTION(BlueprintCallable)
    static bool SetFloatByName(UObject* Target, FName VarName, float NewValue, float& outFloat);
    
    UFUNCTION(BlueprintCallable)
    static bool GetVectorByName(UObject* Target, FName VarName, FVector& outVector);
    
    UFUNCTION(BlueprintCallable)
    static bool GetFloatByName(UObject* Target, FName VarName, float& outFloat);
    
};

