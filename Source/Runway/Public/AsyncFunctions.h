#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AsyncFunctions.generated.h"

UCLASS(Blueprintable)
class RUNWAY_API UAsyncFunctions : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UAsyncFunctions();

    UFUNCTION(BlueprintCallable)
    static void UpdateMeshAsync();
    
};

