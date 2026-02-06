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

UENUM(BlueprintType)
enum class EINV_TileQuadrant : uint8
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
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

USTRUCT(BlueprintType)
struct FINV_TileParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	FIntPoint Coordinates;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	int32 TileIndex{ INDEX_NONE };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	EINV_TileQuadrant TileQuadrant{ EINV_TileQuadrant::None };

};

inline bool operator == (const FINV_TileParameters& A, const FINV_TileParameters& B)
{
	return A.Coordinates == B.Coordinates &&
		A.TileIndex == B.TileIndex &&
		A.TileQuadrant == B.TileQuadrant;
}

USTRUCT()
struct FINV_SpaceQueryResult
{

	GENERATED_BODY()

	//true if the space queried has no items
	bool bHasSpace{ false };

	//Valid if there is single item to swap with
	TWeakObjectPtr<UINV_InventoryItem> ValidItem{ nullptr };

	//index of the valid item if there is one
	int32 UpperLeftIndex{ INDEX_NONE };
};
