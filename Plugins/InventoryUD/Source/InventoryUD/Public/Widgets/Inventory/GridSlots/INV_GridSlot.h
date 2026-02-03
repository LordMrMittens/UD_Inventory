// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "INV_GridSlot.generated.h"

class UImage;
class UINV_InventoryItem;

UENUM(BlueprintType)
enum class EINV_GridSlotState : uint8
{
	Unoccupied,
	Occupied,
	Selected,
	GreyedOut
};

UCLASS()
class INVENTORYUD_API UINV_GridSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetUnoccupiedTexture();
	void SetOccupiedTexture();
	void SetSelectedTexture();
	void SetGreyedOutTexture();
	void SetInventoryItem(UINV_InventoryItem* Item);

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;

	int32 TileIndex;
	int32 StackCount;
	int32 UpperLeftIndex{INDEX_NONE};

	bool bIsAvailable;

	TWeakObjectPtr<UINV_InventoryItem> InventoryItem;

	EINV_GridSlotState GridSlotState = EINV_GridSlotState::Occupied;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Unoccupied;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Occupied;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Selected;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_GreyedOut;




public:
	FORCEINLINE void SetTileIndex(int32 Index) { TileIndex = Index; }
	FORCEINLINE int32 GetTileIndex() const { return TileIndex; }
	FORCEINLINE EINV_GridSlotState GetGridSlotState() { return GridSlotState; }
	FORCEINLINE TWeakObjectPtr<UINV_InventoryItem> GetInventoryItem() { return InventoryItem; }
	FORCEINLINE int32 GetStackCount() const { return StackCount; }
	FORCEINLINE void SetStackCount(int32 Count) { StackCount = Count; }
	FORCEINLINE int32 GetUpperLeftIndex() const { return UpperLeftIndex; }
	FORCEINLINE void SetUpperLeftIndex(int32 Index) { UpperLeftIndex = Index; }
	FORCEINLINE bool GetIsAvailable() const { return bIsAvailable; }
	FORCEINLINE void SetIsAvailable(bool bAvailable) { bIsAvailable = bAvailable; }

};
