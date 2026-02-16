// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/INV_InventoryGrid.h"
#include "Widgets/Inventory/GridSlots/INV_GridSlot.h"
#include "Widgets/SlottedItems/INV_SlottedItem.h"
#include "Widgets/Utils/INV_WidgetUtils.h"
#include "Widgets/ItemPopUp/INV_ItemPopUp.h"
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
#include "InventoryUD.h"
#include "Widgets/Utils/INV_WidgetUtils.h"

void UINV_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ConstructGrid();
	InventoryComponent = UINV_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &UINV_InventoryGrid::AddItem);
	InventoryComponent->OnStackChange.AddDynamic(this, &UINV_InventoryGrid::AddStacks);
}

void UINV_InventoryGrid::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const FVector2D CanvasPos = UINV_WidgetUtils::GetWidgetPosition(CanvasPanel);
	const FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());

	if (CursorExitedCanvas(CanvasPos, UINV_WidgetUtils::GetWidgetSize(CanvasPanel), MousePos)) {
		//Unhighlight slot 
		return;
	}
	UpdateTileParameters(CanvasPos, MousePos);
}

void UINV_InventoryGrid::UpdateTileParameters(const FVector2D& WidgetPosition, const FVector2D& MousePosition) 
{
	if (!bMouseWithinCanvas) return;
	
	LastTileParameters = TileParameters;

	FIntPoint Coordinates = CalculateHoveredCoordinates(WidgetPosition, MousePosition);

	TileParameters.Coordinates = Coordinates;
	TileParameters.TileIndex = UINV_WidgetUtils::GetIndexFromPosition(Coordinates, Columns);
	TileParameters.TileQuadrant = CalculateTileQuadrant(WidgetPosition, MousePosition);
	
	OnTileParametersUpdated(TileParameters);
}


void UINV_InventoryGrid::OnTileParametersUpdated(const FINV_TileParameters& TileParams)
{
	if (!IsValid(HoverItem)) return;

	const FIntPoint Dimensions = HoverItem->GetGridDimensions();

	const FIntPoint StartingCoordinates = CalculateStartingCoordinate(TileParams.Coordinates, Dimensions, TileParams.TileQuadrant);

	ItemDropIndex = UINV_WidgetUtils::GetIndexFromPosition(StartingCoordinates, Columns);
	CurrentQueryResult = CheckHoverPosition(StartingCoordinates, Dimensions);

	if (CurrentQueryResult.bHasSpace)
	{
		HighlightSlots(ItemDropIndex, Dimensions);
		return;
	}
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex)) {
		const FINV_GridFragment* GridFragment = GetFragment<FINV_GridFragment>(CurrentQueryResult.ValidItem.Get(), Fragments::Grid);
		if (!GridFragment) return;

		ChangeHoverType(CurrentQueryResult.UpperLeftIndex, GridFragment->GetGridSize(), EINV_GridSlotState::GreyedOut);
	}
}

FINV_SpaceQueryResult UINV_InventoryGrid::CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions)
{
	FINV_SpaceQueryResult Result;
	if(! IsInGridBounds(UINV_WidgetUtils::GetIndexFromPosition(Position,Columns), Dimensions)) return Result;

	Result.bHasSpace = true;

	//is more than one of the indices occupied with the same item, see if they share the same upper left index
	TSet<int32> OccupiedUpperLeftIndices;
	UINV_InventoryStatics::ForEach2D(GridSlots, UINV_WidgetUtils::GetIndexFromPosition(Position, Columns), Dimensions, Columns, [&](UINV_GridSlot* GridSlot) {
		if (GridSlot->GetInventoryItem().IsValid()) {
			OccupiedUpperLeftIndices.Add(GridSlot->GetUpperLeftIndex());
			Result.bHasSpace = false;
		}
		});
	if (OccupiedUpperLeftIndices.Num() == 1) // Single item, it can swap or combine
	{
		const int32 Index = *OccupiedUpperLeftIndices.CreateConstIterator();
		Result.ValidItem = GridSlots[Index]->GetInventoryItem();
		Result.UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	}

	return Result;
}

bool UINV_InventoryGrid::CursorExitedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize, const FVector2D& Location)
{
	bLastMouseWithinCanvas = bMouseWithinCanvas;
	bMouseWithinCanvas = UINV_WidgetUtils::IsWithinBounds(BoundaryPos, BoundarySize, Location);
	if (!bMouseWithinCanvas && bLastMouseWithinCanvas) {
		UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
		return true;
	}
	return false;
}

void UINV_InventoryGrid::HighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	if (!bMouseWithinCanvas) return;

	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);

	UINV_InventoryStatics::ForEach2D(GridSlots , Index, Dimensions, Columns, [&](UINV_GridSlot* GridSlot) {
		GridSlot->SetOccupiedTexture();
		});

	LastHighlightedDimensions = Dimensions;
	LastHighlightedIndex = Index;
}

void UINV_InventoryGrid::UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	UINV_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UINV_GridSlot* GridSlot) {
		if (GridSlot->GetIsAvailable()) {
			GridSlot->SetUnoccupiedTexture();
		}
		else {

		GridSlot->SetOccupiedTexture();
		}
		});
}

void UINV_InventoryGrid::ChangeHoverType(const int32 Index, const FIntPoint& Dimensions, EINV_GridSlotState GridSlotState)
{
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UINV_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [State = GridSlotState](UINV_GridSlot* GridSlot) {
		switch (State)
		{
		case EINV_GridSlotState::Unoccupied:
			GridSlot->SetUnoccupiedTexture();
			break;
		case EINV_GridSlotState::Occupied:
			GridSlot->SetOccupiedTexture();
			break;
		case EINV_GridSlotState::Selected:
			GridSlot->SetSelectedTexture();
			break;
		case EINV_GridSlotState::GreyedOut:
			GridSlot->SetGreyedOutTexture();
			break;
		default:
			break;
		}
		});
	LastHighlightedIndex = Index;
	LastHighlightedDimensions = Dimensions;
}


FIntPoint UINV_InventoryGrid::CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions, const EINV_TileQuadrant Quadrant) const
{
	const int32 HasEvenWidth = Dimensions.X % 2 == 0 ? 1 : 0;
	const int32 HasEvenHeight = Dimensions.Y % 2 == 0 ? 1 : 0;

	FIntPoint StartingCoord;
	switch (Quadrant)
	{
	case EINV_TileQuadrant::TopLeft:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
	case EINV_TileQuadrant::TopRight:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
	case EINV_TileQuadrant::BottomLeft:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;
	case EINV_TileQuadrant::BottomRight:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;
	default:
		UE_LOG(LogInventory, Error, TEXT("Invalid Quadrant."))
			return FIntPoint(-1, -1);
	}
	return StartingCoord;
}



FIntPoint UINV_InventoryGrid::CalculateHoveredCoordinates(const FVector2D& WidgetPosition, const FVector2D& MousePosition) const
{
	return FIntPoint{
		static_cast<int32>(FMath::FloorToInt((MousePosition.X - WidgetPosition.X) / TileSize)),
		static_cast<int32>(FMath::FloorToInt((MousePosition.Y - WidgetPosition.Y) / TileSize))
	};
}

EINV_TileQuadrant UINV_InventoryGrid::CalculateTileQuadrant(const FVector2D& WidgetPosition, const FVector2D& MousePosition) const
{
	// Calculate relative position within the current tile
	const float TileLocalX = FMath::Fmod(MousePosition.X - WidgetPosition.X, TileSize);
	const float TileLocalY = FMath::Fmod(MousePosition.Y - WidgetPosition.Y, TileSize);

	// Determine which quadrant the mouse is in
	const bool bIsTop = TileLocalY < TileSize / 2.f; // Top if Y is in the upper half
	const bool bIsLeft = TileLocalX < TileSize / 2.f; // Left if X is in the left half

	EINV_TileQuadrant HoveredTileQuadrant{ EINV_TileQuadrant::None };
	if (bIsTop && bIsLeft) HoveredTileQuadrant = EINV_TileQuadrant::TopLeft;
	else if (bIsTop && !bIsLeft) HoveredTileQuadrant = EINV_TileQuadrant::TopRight;
	else if (!bIsTop && bIsLeft) HoveredTileQuadrant = EINV_TileQuadrant::BottomLeft;
	else if (!bIsTop && !bIsLeft) HoveredTileQuadrant = EINV_TileQuadrant::BottomRight;

	return HoveredTileQuadrant;
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
		return;
	}
	if (IsRightClick(MouseEvent)) {
		CreateItemPopUp(GridIndex);
		return;
	}
	if (IsSameStackable(ClickedInventoryItem)) 
		{
		const int32 ClickedStackCount = GridSlots[GridIndex]->GetStackCount();
		const FINV_StackableFragment* StackableFragment = ClickedInventoryItem->GetItemManifest().GetFragmentOfType<FINV_StackableFragment>();
		const int32 MaxStackSize = StackableFragment->GetMaxStackSize();
		const int32 RoomInClickedSlot = MaxStackSize - ClickedStackCount;
		const int32 HoveredStackCount = HoverItem->GetStackCount();
		if (bShouldSwapStackCounts(RoomInClickedSlot, HoveredStackCount, MaxStackSize))
		{
			SwapStackCounts(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}
		if (ShouldConsumeHoverItemStacks(HoveredStackCount, RoomInClickedSlot)) 
		{

			ConsumeHoverItemStacks(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}
		
		if (ShouldFillInStack(RoomInClickedSlot, HoveredStackCount)) 
		{
			FillInStack(RoomInClickedSlot, HoveredStackCount - RoomInClickedSlot, GridIndex);
			return;
		}
		//Slot is full, do nothing
		if (RoomInClickedSlot == 0) return;
	}
	SwapWithHoverItem(ClickedInventoryItem, GridIndex);
}

void UINV_InventoryGrid::CreateItemPopUp(const int32 GridIndex)
{
	UINV_InventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;
	if (IsValid(GridSlots[GridIndex]->GetItemPopUp())) return;

	ItemPopUp = CreateWidget<UINV_ItemPopUp>(this, ItemPopUpClass);
	GridSlots[GridIndex]->SetItemPopUp(ItemPopUp);

	if (OwningCanvasPanel.IsValid())
	{
		OwningCanvasPanel->AddChild(ItemPopUp);

		UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(ItemPopUp);
		if (CanvasSlot)
		{
			const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
			CanvasSlot->SetPosition(MousePosition + ItemPopUpOffset);
			CanvasSlot->SetSize(ItemPopUp->GetBoxSize());

			const int32 SliderMax = GridSlots[GridIndex]->GetStackCount() - 1;
			if (RightClickedItem->IsStackable() && SliderMax > 0) {
				ItemPopUp->PopUpSplit.BindDynamic(this, &ThisClass::OnPopUpMenuSplit);
				ItemPopUp->SetSliderParams(SliderMax, FMath::Max(1, GridSlots[GridIndex]->GetStackCount()/2));
			}
			else {
				ItemPopUp->CollapseSplitButton();
			}
			ItemPopUp->PopUpDrop.BindDynamic(this, &ThisClass::OnPopUpMenuDrop);
			if (RightClickedItem->IsConsumable()) {
				ItemPopUp->PopUpConsume.BindDynamic(this, &ThisClass::OnPopUpMenuConsume);
			}
			else {
				ItemPopUp->CollapseConsumeButton();
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ItemPopUp is not in a Canvas Slot! Check your Hierarchy."));
		}
	}

}

bool UINV_InventoryGrid::ShouldFillInStack(const int32 RoomInClickedSlot, const int32 HoveredStackCount) const
{
	return RoomInClickedSlot < HoveredStackCount;
}

void UINV_InventoryGrid::FillInStack(const int32 FillAmount, const int32 Remainder, const int32 Index)
{
	UINV_GridSlot* GridSlot = GridSlots[Index];
	const int32 NewStackCount = GridSlot->GetStackCount() + FillAmount;

	GridSlot->SetStackCount(NewStackCount);

	UINV_SlottedItem* ClickedSlottedItem = SlottedItems.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(NewStackCount);

	HoverItem->UpdateStackCount(Remainder);

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
			GridSlot->GridSlotClicked.AddDynamic(this, &UINV_InventoryGrid::OnGridSlotClicked);
			GridSlot->GridSlotHovered.AddDynamic(this, &UINV_InventoryGrid::OnGridSlotHovered);
			GridSlot->GridSlotUnhovered.AddDynamic(this, &UINV_InventoryGrid::OnGridSlotUnhovered);
		}
	}

}

void UINV_InventoryGrid::OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (!IsValid(HoverItem)) return;
	if (!GridSlots.IsValidIndex(ItemDropIndex)) return;
	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex)) 
	{
		OnSlottedItemClicked(CurrentQueryResult.UpperLeftIndex, MouseEvent);
		return;
	}
	if (!IsInGridBounds(ItemDropIndex, HoverItem->GetGridDimensions()))return;
	auto GridSlot = GridSlots[ItemDropIndex];
	if (!GridSlot->GetInventoryItem().IsValid()) 
	{
		PutDownOnIndex(ItemDropIndex);
		ClearHoverItem();
	}
}

void UINV_InventoryGrid::PutDownOnIndex(const int32 Index)
{
	AddItemAtIndex(HoverItem->GetInventoryItem(), Index, HoverItem->GetIsStackable(), HoverItem->GetStackCount());
	UpdateGridSlots(HoverItem->GetInventoryItem(), Index, HoverItem->GetIsStackable(), HoverItem->GetStackCount());
}

void UINV_InventoryGrid::SwapWithHoverItem(UINV_InventoryItem* ClickedInventoryItem, const int32 Index)
{
	if (!IsValid(HoverItem)) return;
	UINV_InventoryItem* TempInventoryItem = HoverItem->GetInventoryItem();
	const int32 TempStackCount = HoverItem->GetStackCount();
	const bool bTempIsStackable = HoverItem->GetIsStackable();

	//keep the same previous grid index

	AssignHoverItem(ClickedInventoryItem, Index, HoverItem->GetPreviousGridIndex());
	RemoveItemFromGrid(ClickedInventoryItem, Index);
	AddItemAtIndex(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
	UpdateGridSlots(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
}

bool UINV_InventoryGrid::bShouldSwapStackCounts(const int32 RoomInClickedSlot, const int32 HoveredStackCount, const int32 MaxStackSize) const
{

	return RoomInClickedSlot == 0 && HoveredStackCount < MaxStackSize;
}

void UINV_InventoryGrid::SwapStackCounts(const int32 ClickedStackCount, const int32 HoverStackCount, const int32 Index)
{
	UINV_GridSlot* GridSlot = GridSlots[Index];
	GridSlot->SetStackCount(HoverStackCount);

	UINV_SlottedItem* ClickedSlottedItem = SlottedItems.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(HoverStackCount);

	HoverItem->UpdateStackCount(ClickedStackCount);
}

void UINV_InventoryGrid::ClearHoverItem()
{
	if (!IsValid(HoverItem)) return;
	HoverItem->ClearItem();
	HoverItem = nullptr;
	ShowCursor();
}

UUserWidget* UINV_InventoryGrid::GetVisibleCursorWidget()
{
	if(!IsValid(GetOwningPlayer())) return nullptr;
	if (!IsValid(VisibleCursorWidget))
	{
		VisibleCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), VisibleCursorWidgetClass);
	}
	return VisibleCursorWidget;
}

UUserWidget* UINV_InventoryGrid::GetHiddenCursorWidget()
{
	if (!IsValid(GetOwningPlayer())) return nullptr;
	if (!IsValid(HiddenCursorWidget))
	{
		HiddenCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), HiddenCursorWidgetClass);
	}
	return HiddenCursorWidget;
}
bool UINV_InventoryGrid::IsSameStackable(const UINV_InventoryItem* ClickedInventoryItem) const
{
	const bool bIsSameItem = ClickedInventoryItem == HoverItem->GetInventoryItem();
	const bool bIsStackable = ClickedInventoryItem->IsStackable();
	
	return bIsSameItem && bIsStackable && HoverItem->GetItemType().MatchesTagExact(ClickedInventoryItem->GetItemManifest().GetItemType());
}
bool UINV_InventoryGrid::ShouldConsumeHoverItemStacks(const int32 HoveredStackCount, const int32 RoomInClickedSlot) const
{
	return RoomInClickedSlot >= HoveredStackCount;
}
void UINV_InventoryGrid::ConsumeHoverItemStacks(const int32 ClickedStackCount, const int32 HoverStackCount, const int32 Index)
{
	const int32 AmountToTransfer = HoverStackCount;
	const int32 NewClickedStackCount = ClickedStackCount + AmountToTransfer;

	GridSlots[Index]->SetStackCount(NewClickedStackCount);
	SlottedItems.FindChecked(Index)->UpdateStackCount(NewClickedStackCount);
	ClearHoverItem();
	ShowCursor();

	const FINV_GridFragment* GridFragment = GridSlots[Index]->GetInventoryItem()->GetItemManifest().GetFragmentOfType<FINV_GridFragment>();
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
	HighlightSlots(Index, Dimensions);
}
void UINV_InventoryGrid::ShowCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetVisibleCursorWidget());
}
void UINV_InventoryGrid::HideCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetHiddenCursorWidget());
}

void UINV_InventoryGrid::SetOwningCanvasPanel(UCanvasPanel* OwningCanvas)
{
	OwningCanvasPanel = OwningCanvas;
}

void UINV_InventoryGrid::OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;

	UINV_GridSlot* GridSlot = GridSlots[GridIndex];
	if (GridSlot->GetIsAvailable()) {
		GridSlot->SetOccupiedTexture();
	}
}

void UINV_InventoryGrid::OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;
	UINV_GridSlot* GridSlot = GridSlots[GridIndex];
	if (GridSlot->GetIsAvailable()) {
		GridSlot->SetUnoccupiedTexture();
	}
}

void UINV_InventoryGrid::OnPopUpMenuSplit(int32 SplitAmount, int32 Index)
{
	UINV_InventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();

	if (!IsValid(RightClickedItem)) return;
	if (!RightClickedItem->IsStackable()) return;

	const int32 UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	UINV_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
	const int32 StackCount = UpperLeftGridSlot->GetStackCount();
	const int32 NewStackCount = StackCount - SplitAmount;

	UpperLeftGridSlot->SetStackCount(NewStackCount);
	SlottedItems.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);

	AssignHoverItem(RightClickedItem, UpperLeftIndex, UpperLeftIndex);
	HoverItem->UpdateStackCount(SplitAmount);
}

void UINV_InventoryGrid::OnPopUpMenuDrop(int32 Index)
{
	UINV_InventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();

	if(!IsValid(RightClickedItem)) return;
	PickUp(RightClickedItem, Index);
	DropItem();
}

void UINV_InventoryGrid::DropItem()
{
	if (!IsValid(HoverItem)) return;
	if (!IsValid(HoverItem->GetInventoryItem())) return;


	ClearHoverItem();
	ShowCursor();
}

void UINV_InventoryGrid::OnPopUpMenuConsume(int32 Index)
{

}

bool UINV_InventoryGrid::MatchesCategory(const UINV_InventoryItem* Item) const
{
	return Item->GetItemManifest().GetItemCategory() == ItemCategory;
}
