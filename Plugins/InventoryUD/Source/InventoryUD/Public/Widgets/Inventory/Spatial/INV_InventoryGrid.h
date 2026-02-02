// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/INV_GridTypes.h"
#include "INV_InventoryGrid.generated.h"

class UINV_GridSlot;
class UCanvasPanel;
class UINV_InventoryComponent;
class UINV_InventoryItem;
class UINV_ItemComponent;
class UINV_SlottedItem;
struct FINV_ItemManifest;
struct FINV_GridFragment;
struct FINV_ImageFragment;



UCLASS()
class INVENTORYUD_API UINV_InventoryGrid : public UUserWidget
{

	GENERATED_BODY()


public:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void AddItem(UINV_InventoryItem* Item);

	FINV_SlotAvailabilityResult HasRoomForItem(const UINV_ItemComponent* ItemComponent);

private:

	void ConstructGrid();

	bool MatchesCategory(const UINV_InventoryItem* Item) const;

	FINV_SlotAvailabilityResult HasRoomForItem(const UINV_InventoryItem* Item);
	FINV_SlotAvailabilityResult HasRoomForItem(const FINV_ItemManifest& Manifest);

	void AddItemToIndices(const FINV_SlotAvailabilityResult& AvailabilityResult, UINV_InventoryItem* NewItem);
	void AddItemAtIndex(UINV_InventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount);

	UINV_SlottedItem* CreateSlottedItem(UINV_InventoryItem* Item, 
		const bool bStackable, 
		const int32 StackAmount, 
		const FINV_GridFragment* GridFragment, 
		const FINV_ImageFragment* ImageFragment, 
		const int32 Index);

	void AddSlottedItemToCanvas(const int32 Index, const FINV_GridFragment* GridFragment, UINV_SlottedItem* SlottedItem) const;

	void UpdateGridSlots(UINV_InventoryItem* NewItem, const int32 Index);

	FVector2D GetDrawSize(const FINV_GridFragment* GridFragment) const;

	void SetSlottedItemImage(const UINV_SlottedItem* SlottedItem, const FINV_GridFragment* GridFragment, const FINV_ImageFragment* ImageFragment) const;

	TWeakObjectPtr<UINV_InventoryComponent> InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	EINV_ItemCategory ItemCategory;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UINV_GridSlot> GridSlotClass;

	UPROPERTY()
	TArray<TObjectPtr<UINV_GridSlot>> GridSlots;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UINV_SlottedItem> SlottedItemClass;

	UPROPERTY()
	TMap<int32, TObjectPtr<UINV_SlottedItem>> SlottedItems;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Rows;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Columns;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float TileSize;

public:

	FORCEINLINE EINV_ItemCategory GetItemCategory() const { return ItemCategory; }
};
