// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/INV_InventoryGrid.h"
#include "Widgets/Inventory/GridSlots/INV_GridSlot.h"
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
}

void UINV_InventoryGrid::AddItem(UINV_InventoryItem* Item)
{
	if (!MatchesCategory(Item)) return;

	FINV_SlotAvailabilityResult Result = HasRoomForItem(Item);
	AddItemToIndices(Result, Item);
}

void UINV_InventoryGrid::AddItemToIndices(const FINV_SlotAvailabilityResult& AvailabilityResult, UINV_InventoryItem* NewItem)
{
	const FINV_GridFragment* GridFragment = GetFragment<FINV_GridFragment>(NewItem, Fragments::Grid);
	const FINV_ImageFragment* IconFragment = GetFragment<FINV_ImageFragment>(NewItem, Fragments::Icon);
	if (!GridFragment || !IconFragment) return;
	// TODO Get Fragments to know spaces, icons etc.

	//Create item widget
	//store item widget somewhere
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
	Result.TotalRoomToFill = 1;

	FINV_SlotAvailability SlotAvailability;
	SlotAvailability.AmountToFill = 1;
	SlotAvailability.Index = 0;
	Result.SlotAvailabilities.Add(MoveTemp(SlotAvailability));

	return Result;
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
			GridCPS->SetSize(FVector2D(TileSize,TileSize));
			GridCPS->SetPosition(FVector2D(TilePosition * TileSize));
			GridSlots.Add(GridSlot);
		}
	}

}

bool UINV_InventoryGrid::MatchesCategory(const UINV_InventoryItem* Item) const
{
	return Item->GetItemManifest().GetItemCategory() == ItemCategory;
}
