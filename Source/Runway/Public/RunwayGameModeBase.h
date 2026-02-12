#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RunwayGameModeBase.generated.h"

UCLASS(Blueprintable, NonTransient)
class RUNWAY_API ARunwayGameModeBase : public AGameModeBase {
    GENERATED_BODY()
public:
    ARunwayGameModeBase(const FObjectInitializer& ObjectInitializer);

};

