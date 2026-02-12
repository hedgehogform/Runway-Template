#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "test123.generated.h"

class USplineMeshComponent;

UCLASS(Blueprintable)
class RUNWAY_API Utest123 : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    Utest123();

    UFUNCTION(BlueprintCallable)
    static void UpdateMeshAsync(USplineMeshComponent* splineMeshComp);
    
};

