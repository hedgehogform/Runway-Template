#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "textFileManager.generated.h"

UCLASS(Blueprintable)
class RUNWAY_API UtextFileManager : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UtextFileManager();

    UFUNCTION(BlueprintCallable)
    static bool SaveArrayText(const FString& SaveDirectory, const FString& Filename, TArray<FString> SaveText, bool AllowOverWriting);
    
    UFUNCTION(BlueprintCallable)
    static TArray<FString> LoadArrayText(const FString& LoadDirectory, const FString& Filename);
    
};

