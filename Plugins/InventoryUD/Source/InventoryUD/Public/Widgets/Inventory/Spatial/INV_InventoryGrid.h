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
class UINV_HoverItem;
class UINV_ItemPopUp;
struct FINV_ItemManifest;
struct FINV_GridFragment;
struct FINV_ImageFragment;
struct FGameplayTag;
enum class EINV_GridSlotState : uint8;



UCLASS()
class INVENTORYUD_API UINV_InventoryGrid : public UUserWidget
{

	GENERATED_BODY()


public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void AddItem(UINV_InventoryItem* Item);

	FINV_SlotAvailabilityResult HasRoomForItem(const UINV_ItemComponent* ItemComponent);

	void ShowCursor();
	void HideCursor();
	void SetOwningCanvasPanel(UCanvasPanel* CanvasPanel);

	void DropItem();

	bool HasHoverItem() const;

	UINV_HoverItem* GetHoverItem();

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



	bool HasRoomAtIndex(const UINV_GridSlot* GridSlot, 
		const FIntPoint& Dimensions, 
		const TSet<int32>& CheckedIndices,
		TSet<int32>& OutTentativelyClaimed,
		const FGameplayTag& ItemType,
		const int32 MaxStackSize);

	bool CheckSlotConstraints(const UINV_GridSlot* GridSlot, 
		const UINV_GridSlot* SubGridSlot, 
		const TSet<int32>& CheckedIndices, 
		TSet<int32>& OutTentativelyClaimed, 
		const FGameplayTag& ItemType,
		const int32 MaxStackSize) const;

	bool IsIndexClaimed(const TSet<int32>& CheckedIndices, const int32 Index) const;

	bool HasValidItem(const UINV_GridSlot* GridSlot) const;

	bool IsUpperLeftSlot(const UINV_GridSlot* GridSlot, const UINV_GridSlot* SubGridSlot) const;

	bool DoesItemTypeMatch(const UINV_InventoryItem* SubItem, const FGameplayTag& ItemType) const;

	bool IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const;

	int32 DetermineFillAmountForSlot(const bool bStackable, const int32 MaxStackSize,  const int32 AmountToFill, const UINV_GridSlot* GridSlot) const;

	int32 GetStackAmount(const UINV_GridSlot* GridSlot) const;

	bool IsRightClick(const FPointerEvent& MouseEvent) const;
	bool IsLeftClick(const FPointerEvent& MouseEvent) const;
	void PickUp(UINV_InventoryItem* ClickedItem, const int32 GridIndex);
	void AssignHoverItem(UINV_InventoryItem* InventoryItem);
	void AssignHoverItem(UINV_InventoryItem* InventoryItem, const int32 GridIndex, const int32 PreviousGridIndex );
	void RemoveItemFromGrid(UINV_InventoryItem* InventoryItem, const int32 GridIndex);
	
	FIntPoint GetItemDimensions(const FINV_ItemManifest& Manifest) const;

	void AddSlottedItemToCanvas(const int32 Index, const FINV_GridFragment* GridFragment, UINV_SlottedItem* SlottedItem) const;

	void UpdateGridSlots(UINV_InventoryItem* NewItem, const int32 Index, bool bStackableItem, const int32 StackAmount);

	FVector2D GetDrawSize(const FINV_GridFragment* GridFragment) const;

	void SetSlottedItemImage(const UINV_SlottedItem* SlottedItem, const FINV_GridFragment* GridFragment, const FINV_ImageFragment* ImageFragment) const;

	void UpdateTileParameters(const FVector2D& WidgetPosition,const FVector2D& MousePosition);

	FIntPoint CalculateHoveredCoordinates(const FVector2D& WidgetPosition, const FVector2D& MousePosition) const;

	EINV_TileQuadrant CalculateTileQuadrant(const FVector2D& WidgetPosition, const FVector2D& MousePosition) const;

	void OnTileParametersUpdated(const FINV_TileParameters& TileParameters);
	FIntPoint CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions, const EINV_TileQuadrant Quadrant) const;

	FINV_SpaceQueryResult CheckHoverPosition(const FIntPoint& Position,const FIntPoint& Dimensions );

	bool CursorExitedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize, const FVector2D& Location);

	void HighlightSlots(const int32 Index, const FIntPoint& Dimensions);
	void UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions);
	
	void ChangeHoverType(const int32 Index, const FIntPoint& Dimensions, EINV_GridSlotState GridSlotState);

	void PutDownOnIndex(const int32 Index);

	void SwapWithHoverItem(UINV_InventoryItem* ClickedInventoryItem, const int32 Index);

	bool bShouldSwapStackCounts(const int32 RoomInClickedSlot, const int32 HoveredStackCount, const int32 MaxStackSize) const;

	void SwapStackCounts(const int32 ClickedStackCount, const int32 HoverStackCount, const int32 Index);

	void ClearHoverItem();

	UUserWidget* GetVisibleCursorWidget();
	UUserWidget* GetHiddenCursorWidget();

	bool IsSameStackable(const UINV_InventoryItem* ClickedInventoryItem) const;

	bool ShouldConsumeHoverItemStacks(const int32 HoveredStackCount, const int32 RoomInClickedSlot) const;
	void ConsumeHoverItemStacks(const int32 ClickedStackCount, const int32 HoverStackCount, const int32 Index);
	bool ShouldFillInStack(const int32 RoomInClickedSlot, const int32 HoveredStackCount) const;
	void FillInStack(const int32 FillAmount, const int32 Remainder, const int32 Index);

	void CreateItemPopUp(const int32 GridIndex);

	UFUNCTION()
	void AddStacks(const FINV_SlotAvailabilityResult& Result);
	
	UFUNCTION()
	void OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent);


	UFUNCTION()
	void OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent);
	UFUNCTION()
	void OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent);
	UFUNCTION()
	void OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnPopUpMenuSplit(int32 SplitAmout, int32 Index);
	UFUNCTION()
	void OnPopUpMenuDrop(int32 Index);
	UFUNCTION()
	void OnPopUpMenuConsume(int32 Index);

	

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> VisibleCursorWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> HiddenCursorWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UINV_ItemPopUp> ItemPopUpClass;

	UPROPERTY()
	TObjectPtr<UINV_ItemPopUp> ItemPopUp;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D ItemPopUpOffset = FVector2D{ 10.0f,10.0f };

	UPROPERTY()
	TObjectPtr<UUserWidget> VisibleCursorWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> HiddenCursorWidget;

	TWeakObjectPtr<UINV_InventoryComponent> InventoryComponent;
	TWeakObjectPtr<UCanvasPanel> OwningCanvasPanel;

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

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UINV_HoverItem> HoverItemClass;

	UPROPERTY()
	UINV_HoverItem* HoverItem;

	UPROPERTY()
	TMap<int32, TObjectPtr<UINV_SlottedItem>> SlottedItems;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Rows;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Columns;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float TileSize;

	FINV_TileParameters TileParameters;
	FINV_TileParameters LastTileParameters;

	//Index where an item would be placed if we clicked on the grid at a valid location
	int32 ItemDropIndex{ INDEX_NONE };

	FINV_SpaceQueryResult CurrentQueryResult;
	bool bMouseWithinCanvas;
	bool bLastMouseWithinCanvas;

	int32 LastHighlightedIndex;
	FIntPoint LastHighlightedDimensions;

public:

	FORCEINLINE EINV_ItemCategory GetItemCategory() const { return ItemCategory; }
};
