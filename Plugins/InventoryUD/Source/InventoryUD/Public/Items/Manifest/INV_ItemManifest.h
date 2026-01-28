

#pragma once

#include "CoreMinimal.h"
#include "Types/INV_GridTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "INV_ItemManifest.generated.h"


class UINV_InventoryItem;
/**
 * 
 */
USTRUCT(BlueprintType)
struct INVENTORYUD_API FINV_ItemManifest
{
	GENERATED_BODY()

private:


	UPROPERTY(EditAnywhere, Category = "Inventory")
	EINV_ItemCategory ItemCategory{EINV_ItemCategory::None};

public:

	UINV_InventoryItem* Manifest(UObject* NewOuter);

	FORCEINLINE EINV_ItemCategory GetItemCategory() const { return ItemCategory; }
};
