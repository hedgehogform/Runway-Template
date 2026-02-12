#include "Toolkit/AssetTypeGenerator/CurveBaseGenerator.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveLinearColor.h"
#include "Curves/CurveVector.h"
#include "Dom/JsonObject.h"

UClass *UCurveBaseGenerator::GetAssetObjectClass() const
{
	const FString AssetClassPath = GetAssetData()->GetStringField(TEXT("AssetClass"));
	UClass *FoundClass = FindObject<UClass>(NULL, *AssetClassPath);
	if (!FoundClass)
	{
		UE_LOG(LogAssetGenerator, Error, TEXT("Failed to find curve class: %s"), *AssetClassPath);
		return UCurveBase::StaticClass();
	}
	return FoundClass;
}

void UCurveBaseGenerator::GetAdditionallyHandledAssetClasses(TArray<FName> &OutExtraAssetClasses)
{
	OutExtraAssetClasses.Add(UCurveVector::StaticClass()->GetFName());
	OutExtraAssetClasses.Add(UCurveFloat::StaticClass()->GetFName());
	OutExtraAssetClasses.Add(UCurveLinearColor::StaticClass()->GetFName());
}

FName UCurveBaseGenerator::GetAssetClass()
{
	return UCurveBase::StaticClass()->GetFName();
}
