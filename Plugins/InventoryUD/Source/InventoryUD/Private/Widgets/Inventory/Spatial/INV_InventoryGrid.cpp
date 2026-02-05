// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/INV_InventoryGrid.h"
#include "Widgets/Inventory/GridSlots/INV_GridSlot.h"
#include "Widgets/SlottedItems/INV_SlottedItem.h"
#include "Widgets/Utils/INV_WidgetUtils.h"
#include "Widgets/Inventory/HoverItem/INV_HoverItem.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Items/INV_InventoryItem.h"
#include "Items/Components/INV_ItemComponent.h"
#include "Items/Manifest/INV_ItemManifest.h"
#include "Items/Fragments/INV_ItemFragment.h"
#include "Items/Fragments/INV_FragmentTags.h"
#include "InventoryManagement/Components/INV_InventoryComponent.h"
#include "InventoryManagement/Utils/INV_InventoryStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Widgets/Utils/INV_WidgetUtils.h"

void UINV_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ConstructGrid();
	InventoryComponent = UINV_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &UINV_InventoryGrid::AddItem);
	InventoryComponent->OnStackChange.AddDynamic(this, &UINV_InventoryGrid::AddStacks);
}

void UINV_InventoryGrid::AddItem(UINV_InventoryItem* Item)
{
	if (!MatchesCategory(Item)) return;

	FINV_SlotAvailabilityResult Result = HasRoomForItem(Item);
	AddItemToIndices(Result, Item);
}

void UINV_InventoryGrid::AddItemToIndices(const FINV_SlotAvailabilityResult& AvailabilityResult, UINV_InventoryItem* NewItem)
{
	for (const auto& Availability : AvailabilityResult.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, AvailabilityResult.bStackable, Availability.AmountToFill);
		UpdateGridSlots(NewItem, Availability.Index, AvailabilityResult.bStackable, Availability.AmountToFill);
	}
}

void UINV_InventoryGrid::AddItemAtIndex(UINV_InventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount)
{
	const FINV_GridFragment* GridFragment = GetFragment<FINV_GridFragment>(Item, Fragments::Grid);
	const FINV_ImageFragment* ImageFragment = GetFragment<FINV_ImageFragment>(Item, Fragments::Icon);
	if (!GridFragment || !ImageFragment) return;
	UINV_SlottedItem* SlottedItem = CreateSlottedItem(Item, bStackable, StackAmount, GridFragment, ImageFragment, Index);
	AddSlottedItemToCanvas(Index, GridFragment, SlottedItem);
	SlottedItems.Add(Index, SlottedItem);
}

UINV_SlottedItem* UINV_InventoryGrid::CreateSlottedItem(UINV_InventoryItem* Item, const bool bStackable, const int32 StackAmount, const FINV_GridFragment* GridFragment, const FINV_ImageFragment* ImageFragment, const int32 Index)
{
	UINV_SlottedItem* SlottedItem = CreateWidget<UINV_SlottedItem>(GetOwningPlayer(), SlottedItemClass);
	SlottedItem->SetInventoryItem(Item);

	SetSlottedItemImage(SlottedItem, GridFragment, ImageFragment);
	SlottedItem->SetGridIndex(Index);
	SlottedItem->SetIsStackable(bStackable);
	const int32 StackUpdateAmount = bStackable ? StackAmount : 0;
	SlottedItem->UpdateStackCount(StackUpdateAmount);
	SlottedItem->OnSlottedItemClicked.AddDynamic(this, &UINV_InventoryGrid::OnSlottedItemClicked);
	return SlottedItem;
}


void UINV_InventoryGrid::AddSlottedItemToCanvas(const int32 Index, const FINV_GridFragment* GridFragment, UINV_SlottedItem* SlottedItem) const
{
	CanvasPanel->AddChild(SlottedItem);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(SlottedItem);
	CanvasSlot->SetSize(GetDrawSize(GridFragment));
	const FVector2D DrawPos = UINV_WidgetUtils::GetPositionFromIndex(Index, Columns) * TileSize;
	const FVector2D DrawPosWithPadding = DrawPos + FVector2D(GridFragment->GetGridPadding());
	CanvasSlot->SetPosition(DrawPosWithPadding);
}

void UINV_InventoryGrid::UpdateGridSlots(UINV_InventoryItem* NewItem, const int32 Index, bool bStackableItem, const int32 StackAmount)
{
	check(GridSlots.IsValidIndex(Index));

	if (bStackableItem) {
		GridSlots[Index]->SetStackCount(StackAmount);
	}


	const FINV_GridFragment* GridFragment = GetFragment<FINV_GridFragment>(NewItem, Fragments::Grid);
	if (!GridFragment) return;
	GridFragment->GetGridSize();
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);

	UINV_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UINV_GridSlot* GridSlot)
		{
			GridSlot->SetInventoryItem(NewItem);
			GridSlot->SetUpperLeftIndex(Index);
			GridSlot->SetOccupiedTexture();
			GridSlot->SetIsAvailable(false);
		});

}

FVector2D UINV_InventoryGrid::GetDrawSize(const FINV_GridFragment* GridFragment) const
{
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
	return GridFragment->GetGridSize() * IconTileWidth;
}

void UINV_InventoryGrid::SetSlottedItemImage(const UINV_SlottedItem* SlottedItem, const FINV_GridFragment* GridFragment, const FINV_ImageFragment* ImageFragment) const
{
	FSlateBrush Brush;

	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;

	Brush.ImageSize = GetDrawSize(GridFragment);
	SlottedItem->SetImageBrush(Brush);
}

void UINV_InventoryGrid::AddStacks(const FINV_SlotAvailabilityResult& Result)
{
	if (!MatchesCategory(Result.Item.Get())) return;

	for (const auto& Availability : Result.SlotAvailabilities) {
		if (Availability.bItemAtIndex) {
			const auto& Gridslot = GridSlots[Availability.Index];
			const auto& SlottedItem = SlottedItems.FindChecked(Availability.Index);
			SlottedItem->UpdateStackCount(Gridslot->GetStackCount() + Availability.AmountToFill);
			Gridslot->SetStackCount(Gridslot->GetStackCount() + Availability.AmountToFill);
		}
		else
		{
			AddItemAtIndex(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
			UpdateGridSlots(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
		}
	}
}

void UINV_InventoryGrid::OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	check(GridSlots.IsValidIndex(GridIndex));
	UINV_InventoryItem* ClickedInventoryItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(HoverItem) && IsLeftClick(MouseEvent)) 
	{
		PickUp(ClickedInventoryItem, GridIndex);
		
	}
}

FINV_SlotAvailabilityResult UINV_InventoryGrid::HasRoomForItem(const UINV_ItemComponent* ItemComponent)
{

	return HasRoomForItem(ItemComponent->GetItemManifest());
}

FINV_SlotAvailabilityResult UINV_InventoryGrid::HasRoomForItem(const UINV_InventoryItem* Item)
{
	return HasRoomForItem(Item->GetItemManifest());
}

FINV_SlotAvailabilityResult UINV_InventoryGrid::HasRoomForItem(const FINV_ItemManifest& Manifest)
{
	FINV_SlotAvailabilityResult Result;

	//Is the item stackable
	const FINV_StackableFragment* StackableFragment = Manifest.GetFragmentOfType<FINV_StackableFragment>();
	Result.bStackable = StackableFragment != nullptr;

	//If stackable how many stacks to add
	const int32 MaximumStackSize = StackableFragment ? StackableFragment->GetMaxStackSize() : 1;
	int32 AmountToFill = StackableFragment ? StackableFragment->GetStackCount() : 1;

	TSet<int32> CheckedIndices;
	//loop over grid slots and 
	for (const auto& GridSlot : GridSlots)
	{
		//check if there is amount to fill if not break out
		if (AmountToFill == 0) break;

		//Is this Index claimed
		//****there is a difference here in code between me and lesson Gridslot checks tile index not index but should be the same***
		if (IsIndexClaimed(CheckedIndices, GridSlot->GetTileIndex())) continue;

		//Is the item GridBounds
		if (!IsInGridBounds(GridSlot->GetTileIndex(), GetItemDimensions(Manifest))) continue;

		//Can Item fit here (IE is there room within the bounds of grid)
		TSet<int32> TentativelyClaimed;
		if (!HasRoomAtIndex(GridSlot, GetItemDimensions(Manifest), CheckedIndices, TentativelyClaimed, Manifest.GetItemType(), MaximumStackSize))
		{
			continue;
		}

		const int32 AmountToFillInSlot = DetermineFillAmountForSlot(Result.bStackable, MaximumStackSize, AmountToFill, GridSlot);
	
		//How much to fill?
		if (AmountToFillInSlot == 0) continue;

		CheckedIndices.Append(TentativelyClaimed);

		//Update the amount left to fill
		Result.TotalRoomToFill += AmountToFillInSlot;
		Result.SlotAvailabilities.Emplace(
			FINV_SlotAvailability{
				HasValidItem(GridSlot) ? GridSlot->GetUpperLeftIndex() : GridSlot->GetTileIndex(),
				Result.bStackable ? AmountToFillInSlot : 0,
				HasValidItem(GridSlot)
			}
		);
		AmountToFill -= AmountToFillInSlot;
		
		//what is the remainder
		Result.Remainder = AmountToFill;
		if (AmountToFill == 0) return Result;
	}


	return Result;
}

bool UINV_InventoryGrid::HasRoomAtIndex(const UINV_GridSlot* GridSlot, const FIntPoint& Dimensions, const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, const int32 MaxStackSize)
{
	//is there room at the index (IE are there other items in the way?)
	bool bHasRoomAtIndex{ true };
	UINV_InventoryStatics::ForEach2D(GridSlots, GridSlot->GetTileIndex(), Dimensions, Columns, [&](const UINV_GridSlot* SubGridSlot)
		{
			if (CheckSlotConstraints(GridSlot, SubGridSlot, CheckedIndices, OutTentativelyClaimed, ItemType, MaxStackSize)) {
				OutTentativelyClaimed.Add(SubGridSlot->GetTileIndex());
			}
			else 
			{
				bHasRoomAtIndex = false;
			}
		});
	return bHasRoomAtIndex;
}

bool UINV_InventoryGrid::CheckSlotConstraints(const UINV_GridSlot* GridSlot, const UINV_GridSlot* SubGridSlot, const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, const int32 MaxStackSize) const
{
	//Index claimed (for each in range2D)
	if (IsIndexClaimed(CheckedIndices, SubGridSlot->GetTileIndex())) return false;
	//Has valid item
	if (!HasValidItem(SubGridSlot)) {
		OutTentativelyClaimed.Add(SubGridSlot->GetTileIndex());
		return true;
	}
	// is this the upper left slot
	if (!IsUpperLeftSlot(GridSlot, SubGridSlot)) return false;
	//if so, is it stackable
	const UINV_InventoryItem* SubItem = SubGridSlot->GetInventoryItem().Get();
	if (!SubItem->IsStackable()) return false;
	//Is the item the same type as the item we're trying to add
	if (!DoesItemTypeMatch(SubItem, ItemType)) return false;
	//if so, is the slot at the max stack size?
	if (GridSlot->GetStackCount() >= MaxStackSize) return false;
	return true;
}

bool UINV_InventoryGrid::IsIndexClaimed(const TSet<int32>& CheckedIndices, const int32 Index) const
{

	return CheckedIndices.Contains(Index);
}

bool UINV_InventoryGrid::HasValidItem(const UINV_GridSlot* GridSlot) const
{
	return GridSlot->GetInventoryItem().IsValid();
}

bool UINV_InventoryGrid::IsUpperLeftSlot(const UINV_GridSlot* GridSlot, const UINV_GridSlot* SubGridSlot) const
{

	return GridSlot->GetUpperLeftIndex() == SubGridSlot->GetUpperLeftIndex();
}

bool UINV_InventoryGrid::DoesItemTypeMatch(const UINV_InventoryItem* SubItem, const FGameplayTag& ItemType) const
{
	return SubItem->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
}

bool UINV_InventoryGrid::IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const
{
	if (StartIndex < 0 || StartIndex >= GridSlots.Num()) return false;
	const int32 EndColumn = (StartIndex % Columns) + ItemDimensions.X;
	const int32 EndRow = (StartIndex / Columns) + ItemDimensions.Y;
	return EndColumn <= Columns, EndRow <= Rows;
}

int32 UINV_InventoryGrid::DetermineFillAmountForSlot(const bool bStackable, const int32 MaxStackSize, const int32 AmountToFill, const UINV_GridSlot* GridSlot) const
{
	//calculate room in slot
	const int32 RoomInSlot = MaxStackSize - GetStackAmount(GridSlot);
	//if stackable, min between amount to fill and room in slot
	return bStackable ? FMath::Min(AmountToFill, RoomInSlot) : 1;
}

int32 UINV_InventoryGrid::GetStackAmount(const UINV_GridSlot* GridSlot) const
{
	int32 CurrentSlotStackCount = GridSlot->GetStackCount();
	//if not upper left slot, slot does not hold stack count
	//if not holding stack count get actual stack count
	if (const int32 UpperLeftIndex = GridSlot->GetUpperLeftIndex(); UpperLeftIndex != INDEX_NONE) 
	{
		UINV_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
		CurrentSlotStackCount = UpperLeftGridSlot->GetStackCount();
	}
	return CurrentSlotStackCount;
}

bool UINV_InventoryGrid::IsRightClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
}

bool UINV_InventoryGrid::IsLeftClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
}

void UINV_InventoryGrid::PickUp(UINV_InventoryItem* ClickedItem, const int32 GridIndex)
{
	AssignHoverItem(ClickedItem, GridIndex, GridIndex);
	RemoveItemFromGrid(ClickedItem, GridIndex);
}

void UINV_InventoryGrid::AssignHoverItem(UINV_InventoryItem* InventoryItem)
{
	if (!IsValid(HoverItem)) {
		HoverItem = CreateWidget<UINV_HoverItem>(GetOwningPlayer(), HoverItemClass);
	}
	const FINV_GridFragment* GridFragment = GetFragment<FINV_GridFragment>(InventoryItem, Fragments::Grid);
	const FINV_ImageFragment* ImageFragment = GetFragment<FINV_ImageFragment>(InventoryItem, Fragments::Icon);
	if (!GridFragment || !ImageFragment) return;
	const FVector2D DrawSize = GetDrawSize(GridFragment);
	FSlateBrush IconBrush;
	IconBrush.SetResourceObject(ImageFragment->GetIcon());
	IconBrush.DrawAs = ESlateBrushDrawType::Image;
	IconBrush.ImageSize = DrawSize * UWidgetLayoutLibrary::GetViewportScale(this);

	HoverItem->SetImageBrush(IconBrush);
	HoverItem->SetGridDimensions(GridFragment->GetGridSize());
	HoverItem->SetInventoryItem(InventoryItem);
	HoverItem->SetIsStackable(InventoryItem->IsStackable());

	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, HoverItem);
}

void UINV_InventoryGrid::AssignHoverItem(UINV_InventoryItem* InventoryItem, const int32 GridIndex, const int32 PreviousGridIndex)
{
	AssignHoverItem(InventoryItem);
	HoverItem->SetPreviousGridIndex(PreviousGridIndex);
	HoverItem->UpdateStackCount(InventoryItem->IsStackable() ? GridSlots[GridIndex]->GetStackCount() : 0);
}

void UINV_InventoryGrid::RemoveItemFromGrid(UINV_InventoryItem* InventoryItem, const int32 GridIndex)
{
	const FINV_GridFragment* GridFragment = GetFragment<FINV_GridFragment>(InventoryItem, Fragments::Grid);
	if (!GridFragment) return;

	UINV_InventoryStatics::ForEach2D(GridSlots, GridIndex, GridFragment->GetGridSize(), Columns,[&](UINV_GridSlot* GridSlot) {
		GridSlot->SetInventoryItem(nullptr);
		GridSlot->SetUpperLeftIndex(INDEX_NONE);
		GridSlot->SetUnoccupiedTexture();
		GridSlot->SetIsAvailable(true);
		GridSlot->SetStackCount(0);
	});

	if (SlottedItems.Contains(GridIndex)) {
		TObjectPtr<UINV_SlottedItem> FoundSlottedItem;
		SlottedItems.RemoveAndCopyValue(GridIndex, FoundSlottedItem);
		FoundSlottedItem->RemoveFromParent();
	}
}

FIntPoint UINV_InventoryGrid::GetItemDimensions(const FINV_ItemManifest& Manifest) const
{
	const FINV_GridFragment* GridFragment = Manifest.GetFragmentOfType<FINV_GridFragment>();
	return GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
}


void UINV_InventoryGrid::ConstructGrid()
{
	GridSlots.Reserve(Rows * Columns);

	for (int32 j = 0; j < Rows; j++)
	{
		for (int32 i = 0; i < Columns; i++) {
			UINV_GridSlot* GridSlot = CreateWidget<UINV_GridSlot>(this, GridSlotClass);
			CanvasPanel->AddChild(GridSlot);
			const FIntPoint TilePosition(i, j);
			GridSlot->SetTileIndex(UINV_WidgetUtils::GetIndexFromPosition(TilePosition, Columns));
			UCanvasPanelSlot* GridCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(GridSlot);
			GridCPS->SetSize(FVector2D(TileSize, TileSize));
			GridCPS->SetPosition(FVector2D(TilePosition * TileSize));
			GridSlots.Add(GridSlot);
		}
	}

}

bool UINV_InventoryGrid::MatchesCategory(const UINV_InventoryItem* Item) const
{
	return Item->GetItemManifest().GetItemCategory() == ItemCategory;
}
