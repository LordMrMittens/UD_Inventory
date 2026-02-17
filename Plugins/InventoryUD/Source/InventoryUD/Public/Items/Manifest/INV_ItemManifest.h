

#pragma once

#include "CoreMinimal.h"
#include "Types/INV_GridTypes.h"
#include "Items/INV_ItemTags.h"
#include "StructUtils/InstancedStruct.h"
#include "GameplayTagContainer.h"
#include "INV_ItemManifest.generated.h"


class UINV_InventoryItem;
struct FINV_ItemFragment;
/**
 * 
 */
USTRUCT(BlueprintType)
struct INVENTORYUD_API FINV_ItemManifest

{
	GENERATED_BODY()
public:
	template<typename T> requires std::derived_from<T, FINV_ItemFragment>
	const T* GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const;

	template<typename T> requires std::derived_from<T, FINV_ItemFragment>
	const T* GetFragmentOfType() const;

	template<typename T> requires std::derived_from<T, FINV_ItemFragment>
	T* GetFragmentOfTypeMutable();

	void SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation, const FRotator& SpawnRotation);

private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FINV_ItemFragment>> ItemFragments;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	EINV_ItemCategory ItemCategory{ EINV_ItemCategory::None };

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories="GameItems"))
	FGameplayTag ItemType;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<AActor> PickupActorClass;
	

public:

	UINV_InventoryItem* Manifest(UObject* NewOuter);

	FORCEINLINE EINV_ItemCategory GetItemCategory() const { return ItemCategory; }
	FORCEINLINE FGameplayTag GetItemType() const {return ItemType;}
};

template<typename T> requires std::derived_from<T, FINV_ItemFragment>
const T* FINV_ItemManifest::GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const
{
	for (const TInstancedStruct<FINV_ItemFragment>& Fragment : ItemFragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>()) {
			if (!FragmentPtr->GetFragmentTag().MatchesTagExact(FragmentTag)) continue;
			return FragmentPtr;
		}
	}

	return nullptr;
};

template<typename T> requires std::derived_from<T, FINV_ItemFragment>
const T* FINV_ItemManifest::GetFragmentOfType() const
{
	for (const TInstancedStruct<FINV_ItemFragment>& Fragment : ItemFragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			return FragmentPtr;
		}
	}

	return nullptr;
};

template<typename T> requires std::derived_from<T, FINV_ItemFragment>
inline T* FINV_ItemManifest::GetFragmentOfTypeMutable()
{
	for (TInstancedStruct<FINV_ItemFragment>& Fragment : ItemFragments)
	{
		if (T* FragmentPtr = Fragment.GetMutablePtr<T>())
		{
			return FragmentPtr;
		}
	}

	return nullptr;
};
