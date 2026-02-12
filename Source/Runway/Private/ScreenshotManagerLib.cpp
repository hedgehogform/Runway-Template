#include "ScreenshotManagerLib.h"

UScreenshotManagerLib::UScreenshotManagerLib() {
}

FString UScreenshotManagerLib::thumbnailToString(const FString& thumbnameFilenamePath) {
    return TEXT("");
}

void UScreenshotManagerLib::TakeScreenshot(const FString& filenamePath) {
}

UTexture2D* UScreenshotManagerLib::saveThumbnailToFileFromString(const FString& imageStringData, const FString& pathToSaveTo) {
    return NULL;
}

UTexture2D* UScreenshotManagerLib::LoadThumbnail(const FString& filenamePath) {
    return NULL;
}

UTexture2D* UScreenshotManagerLib::LoadScreenshot(const FString& filenamePath) {
    return NULL;
}

void UScreenshotManagerLib::getTextureFromRawImageAsync(FTestDelegate Out, const FString& imageStringData, const FString& pathToSaveTo) {
}


