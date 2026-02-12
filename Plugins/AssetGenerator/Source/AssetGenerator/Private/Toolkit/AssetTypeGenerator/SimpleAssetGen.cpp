#include "Toolkit/AssetTypeGenerator/SimpleAssetGen.h"
#include "Dom/JsonObject.h"

UClass *USimpleAssetGen::GetAssetObjectClass() const
{
	const FString AssetClassPath = GetAssetData()->GetStringField(TEXT("AssetClass"));
	UClass *FoundClass = FindObject<UClass>(NULL, *AssetClassPath);
	if (!FoundClass)
	{
		UE_LOG(LogAssetGenerator, Error, TEXT("Failed to find class: %s"), *AssetClassPath);
		return UObject::StaticClass();
	}
	return FoundClass;
}

FName USimpleAssetGen::GetAssetClass()
{
	return TEXT("SimpleAsset"); // UPhysicalMaterial::StaticClass()->GetFName();
}

void USimpleAssetGen::PopulateStageDependencies(TArray<FPackageDependency> &OutDependencies) const
{
	if (!GetAssetData()->HasField(TEXT("SkipDependecies")) || !GetAssetData()->GetBoolField(TEXT("SkipDependecies")))
	{
		if (GetCurrentStage() == EAssetGenerationStage::CONSTRUCTION)
		{
			PopulateReferencedObjectsDependencies(OutDependencies);
		}
	}
}