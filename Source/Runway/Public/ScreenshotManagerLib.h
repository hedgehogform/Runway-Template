#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TestDelegateDelegate.h"
#include "ScreenshotManagerLib.generated.h"

class UTexture2D;

UCLASS(Blueprintable)
class RUNWAY_API UScreenshotManagerLib : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UScreenshotManagerLib();

    UFUNCTION(BlueprintCallable)
    static FString thumbnailToString(const FString& thumbnameFilenamePath);
    
    UFUNCTION(BlueprintCallable)
    static void TakeScreenshot(const FString& filenamePath);
    
    UFUNCTION(BlueprintCallable)
    static UTexture2D* saveThumbnailToFileFromString(const FString& imageStringData, const FString& pathToSaveTo);
    
    UFUNCTION(BlueprintCallable)
    static UTexture2D* LoadThumbnail(const FString& filenamePath);
    
    UFUNCTION(BlueprintCallable)
    static UTexture2D* LoadScreenshot(const FString& filenamePath);
    
    UFUNCTION(BlueprintCallable)
    static void getTextureFromRawImageAsync(FTestDelegate Out, const FString& imageStringData, const FString& pathToSaveTo);
    
};

