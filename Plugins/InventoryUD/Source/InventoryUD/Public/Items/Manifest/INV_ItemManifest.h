

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

private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FINV_ItemFragment>> ItemFragments;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	EINV_ItemCategory ItemCategory{ EINV_ItemCategory::None };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag ItemType;

public:

	UINV_InventoryItem* Manifest(UObject* NewOuter);

	FORCEINLINE EINV_ItemCategory GetItemCategory() const { return ItemCategory; }
	FORCEINLINE FGameplayTag GetItemType() const {return ItemType;}
};
