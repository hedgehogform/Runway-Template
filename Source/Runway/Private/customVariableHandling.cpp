#include "customVariableHandling.h"

UcustomVariableHandling::UcustomVariableHandling() {
}

bool UcustomVariableHandling::SetVectorByName(UObject* Target, FName VarName, FVector NewValue) {
    return false;
}

bool UcustomVariableHandling::SetFloatByName(UObject* Target, FName VarName, float NewValue, float& outFloat) {
    return false;
}

bool UcustomVariableHandling::GetVectorByName(UObject* Target, FName VarName, FVector& outVector) {
    return false;
}

bool UcustomVariableHandling::GetFloatByName(UObject* Target, FName VarName, float& outFloat) {
    return false;
}


