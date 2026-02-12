#include "Toolkit/AssetTypeGenerator/BlendSpaceGenerator.h"
#include "Animation/AimOffsetBlendSpace.h"
#include "Animation/AimOffsetBlendSpace1D.h"
#include "Animation/BlendSpaceBase.h"
#include "Dom/JsonObject.h"
#include "Toolkit/ObjectHierarchySerializer.h"

UClass *UBlendSpaceGenerator::GetAssetObjectClass() const
{
	const FString AssetClassPath = GetAssetData()->GetStringField(TEXT("AssetClass"));
	UClass *FoundClass = FindObject<UClass>(NULL, *AssetClassPath);
	if (!FoundClass)
	{
		UE_LOG(LogAssetGenerator, Error, TEXT("Failed to find blend space class: %s"), *AssetClassPath);
		return UBlendSpaceBase::StaticClass();
	}
	return FoundClass;
}

void UBlendSpaceGenerator::PopulateSimpleAssetWithData(UObject *Asset)
{
	const TSharedPtr<FJsonObject> AssetData = GetAssetData();
	const TSharedPtr<FJsonObject> AssetObjectProperties = AssetData->GetObjectField(TEXT("AssetObjectData"));
	if (!AssetObjectProperties.IsValid())
	{
		UE_LOG(LogAssetGenerator, Error, TEXT("AssetObjectData field is missing or invalid in BlendSpace"));
		return;
	}

	TArray<TSharedPtr<FJsonValue>> SampleData = AssetObjectProperties->GetArrayField(TEXT("SampleData"));
	if (SampleData.Num() > 0)
	{
		TArray<TSharedPtr<FJsonValue>> ObjectsToRemove;

		for (int i = 0; i < SampleData.Num(); i++)
		{
			const TSharedPtr<FJsonObject> SampleDataObject = SampleData[i]->AsObject();
			const int32 AnimationIndex = SampleDataObject->GetIntegerField(TEXT("Animation"));

			if (AnimationIndex != -1)
			{
				const UAnimationAsset *TargetAnimation = Cast<UAnimationAsset>(GetObjectSerializer()->DeserializeObject(AnimationIndex));

				if (!TargetAnimation)
				{
					ObjectsToRemove.Add(SampleData[i]);
				}
			}
		}

		for (int i = 0; i < ObjectsToRemove.Num(); i++)
		{
			SampleData.Remove(ObjectsToRemove[i]);
		}
	}

	AssetObjectProperties->SetArrayField(TEXT("SampleData"), SampleData);

	GetObjectSerializer()->DeserializeObjectProperties(AssetObjectProperties.ToSharedRef(), Asset);
}

void UBlendSpaceGenerator::GetAdditionallyHandledAssetClasses(TArray<FName> &OutExtraAssetClasses)
{
	OutExtraAssetClasses.Add(UBlendSpace::StaticClass()->GetFName());
	OutExtraAssetClasses.Add(UBlendSpace1D::StaticClass()->GetFName());
	OutExtraAssetClasses.Add(UAimOffsetBlendSpace::StaticClass()->GetFName());
	OutExtraAssetClasses.Add(UAimOffsetBlendSpace1D::StaticClass()->GetFName());
}

FName UBlendSpaceGenerator::GetAssetClass()
{
	return UBlendSpaceBase::StaticClass()->GetFName();
}