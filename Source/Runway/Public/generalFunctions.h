#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "generalFunctions.generated.h"

class UObject;

UCLASS(Blueprintable)
class RUNWAY_API UgeneralFunctions : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UgeneralFunctions();

    UFUNCTION(BlueprintCallable)
    static void recursivelyDeleteDirectory(const FString& CompletePath);
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static void offsetLocationOfAllActors(const UObject* WorldContextObject, FVector Offset);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FQuat GetQuatAxisAngle(FVector Axis, float AngleRad);
    
};

