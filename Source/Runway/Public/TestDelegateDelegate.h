#pragma once
#include "CoreMinimal.h"
#include "TestDelegateDelegate.generated.h"

class UTexture2D;

UDELEGATE(BlueprintCallable) DECLARE_DYNAMIC_DELEGATE_OneParam(FTestDelegate, UTexture2D*, outTex);

