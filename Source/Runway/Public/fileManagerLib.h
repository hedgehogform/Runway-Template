#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "fileManagerLib.generated.h"

UCLASS(Blueprintable)
class RUNWAY_API UfileManagerLib : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UfileManagerLib();

    UFUNCTION(BlueprintCallable)
    static bool renameFileUTF8(const FString& DirectoryPath, const FString& OldName, const FString& NewName);
    
    UFUNCTION(BlueprintCallable)
    static bool renameFile(const FString& DirectoryPath, const FString& OldName, const FString& NewName);
    
    UFUNCTION(BlueprintCallable)
    static TArray<FString> findFilesInDirectory(const FString& Path);
    
};

