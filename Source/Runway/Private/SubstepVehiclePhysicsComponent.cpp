#include "SubstepVehiclePhysicsComponent.h"

USubstepVehiclePhysicsComponent::USubstepVehiclePhysicsComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
}

void USubstepVehiclePhysicsComponent::SetVelocity(FVector vel, float LinearDamping) {
}

void USubstepVehiclePhysicsComponent::SetTransform(FTransform NewTransform) {
}

void USubstepVehiclePhysicsComponent::ResetPhysics(bool simulatePhysics) {
}

FVector USubstepVehiclePhysicsComponent::GetWorldCenterOfMass() {
    return FVector{};
}

FVector USubstepVehiclePhysicsComponent::GetVelocity() {
    return FVector{};
}

FTransform USubstepVehiclePhysicsComponent::GetTransform() {
    return FTransform{};
}

FQuat USubstepVehiclePhysicsComponent::GetQuatRotation() {
    return FQuat{};
}

void USubstepVehiclePhysicsComponent::AddTorqueAtLocation(FVector Torque, bool accelChange) {
}

void USubstepVehiclePhysicsComponent::AddForceAtLocation(FVector Force, FVector Location) {
}

void USubstepVehiclePhysicsComponent::AddForce(FVector Force, bool accelChange) {
}


