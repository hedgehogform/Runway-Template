#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "Components/ActorComponent.h"
#include "SubstepVehiclePhysicsComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class RUNWAY_API USubstepVehiclePhysicsComponent : public UActorComponent {
    GENERATED_BODY()
public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMyStuffUsed, const FVector&, DeltaTime);
    
    UPROPERTY(BlueprintAssignable, BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FOnMyStuffUsed OnMyStuffUsed;
    
    USubstepVehiclePhysicsComponent(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    void SetVelocity(FVector vel, float LinearDamping);
    
    UFUNCTION(BlueprintCallable)
    void SetTransform(FTransform NewTransform);
    
    UFUNCTION(BlueprintCallable)
    void ResetPhysics(bool simulatePhysics);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FVector GetWorldCenterOfMass();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FVector GetVelocity();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FTransform GetTransform();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FQuat GetQuatRotation();
    
    UFUNCTION(BlueprintCallable)
    void AddTorqueAtLocation(FVector Torque, bool accelChange);
    
    UFUNCTION(BlueprintCallable)
    void AddForceAtLocation(FVector Force, FVector Location);
    
    UFUNCTION(BlueprintCallable)
    void AddForce(FVector Force, bool accelChange);
    
};

