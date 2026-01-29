#pragma once

#include "INV_GridTypes.generated.h"

class UINV_InventoryItem;

UENUM(BlueprintType)
enum class  EINV_ItemCategory : uint8
{
	Equippable,
	Consumable,
	Craftable,
	None
};
USTRUCT()
struct FINV_SlotAvailability {
	GENERATED_BODY()

	FINV_SlotAvailability() {}
	FINV_SlotAvailability(int32 ItemIndex, int32 Room, bool bHasItem) : Index(ItemIndex), AmountToFill(Room), bItemAtIndex(bHasItem) {}

	int32 Index{ INDEX_NONE };
	int32 AmountToFill{ 0 };
	bool bItemAtIndex{ false };
};


USTRUCT()
struct FINV_SlotAvailabilityResult {
	GENERATED_BODY()

	FINV_SlotAvailabilityResult() {}

	TWeakObjectPtr<UINV_InventoryItem> Item;

	bool bStackable{ false };
	int32 TotalRoomToFill{ 0 };
	int32 Remainder{ 0 };

	TArray<FINV_SlotAvailability> SlotAvailabilities;
};