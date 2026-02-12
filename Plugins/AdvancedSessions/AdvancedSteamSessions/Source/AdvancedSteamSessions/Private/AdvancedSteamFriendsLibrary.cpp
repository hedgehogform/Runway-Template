#include "AdvancedSteamFriendsLibrary.h"

UAdvancedSteamFriendsLibrary::UAdvancedSteamFriendsLibrary() {
}

bool UAdvancedSteamFriendsLibrary::RequestSteamFriendInfo(const FBPUniqueNetId UniqueNetId, bool bRequireNameOnly) {
    return false;
}

bool UAdvancedSteamFriendsLibrary::OpenSteamUserOverlay(const FBPUniqueNetId UniqueNetId, ESteamUserOverlayType DialogType) {
    return false;
}

FString UAdvancedSteamFriendsLibrary::GetSteamPersonaName(const FBPUniqueNetId UniqueNetId) {
    return TEXT("");
}

void UAdvancedSteamFriendsLibrary::GetSteamGroups(TArray<FBPSteamGroupInfo>& SteamGroups) {
}

void UAdvancedSteamFriendsLibrary::GetSteamFriendGamePlayed(const FBPUniqueNetId UniqueNetId, EBlueprintResultSwitch& Result, int32& AppId) {
}

UTexture2D* UAdvancedSteamFriendsLibrary::GetSteamFriendAvatar(const FBPUniqueNetId UniqueNetId, EBlueprintAsyncResultSwitch& Result, SteamAvatarSize AvatarSize) {
    return NULL;
}

FBPUniqueNetId UAdvancedSteamFriendsLibrary::GetLocalSteamIDFromSteam() {
    return FBPUniqueNetId{};
}

int32 UAdvancedSteamFriendsLibrary::GetFriendSteamLevel(const FBPUniqueNetId UniqueNetId) {
    return 0;
}

FBPUniqueNetId UAdvancedSteamFriendsLibrary::CreateSteamIDFromString(const FString& SteamID64) {
    return FBPUniqueNetId{};
}


