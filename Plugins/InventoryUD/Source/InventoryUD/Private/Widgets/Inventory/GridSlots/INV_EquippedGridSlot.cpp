// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/GridSlots/INV_EquippedGridSlot.h"
#include "InventoryManagement/Utils/INV_InventoryStatics.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "Components/Image.h"
#include "Items/INV_InventoryItem.h"
#include "Items/Fragments/INV_ItemFragment.h"
#include "Widgets/Inventory/HoverItem/INV_HoverItem.h"

void UINV_EquippedGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	if (!GetIsAvailable()) return;
	UINV_HoverItem* HoverItem = UINV_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem)) return;

	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		SetOccupiedTexture();
		Image_GreyedOutIcon->SetVisibility(ESlateVisibility::Collapsed);
	}

}

void UINV_EquippedGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{

	if (!GetIsAvailable()) return;
	UINV_HoverItem* HoverItem = UINV_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem)) return;

	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		SetUnoccupiedTexture();
		Image_GreyedOutIcon->SetVisibility(ESlateVisibility::Visible);
	}
}

FReply UINV_EquippedGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	EquippedGridSlotClicked.Broadcast(this, EquipmentTypeTag);
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

UINV_EquippedSlottedItem* UINV_EquippedGridSlot::OnItemEquipped(UINV_InventoryItem* Item, const FGameplayTag& EquipmentTag, float TileSize)
{
	if (!EquipmentTag.MatchesTagExact(EquipmentTypeTag)) return nullptr;
	const FINV_GridFragment* GridFragment = GetFragment<FINV_GridFragment>(Item, Fragments::Grid); 
	if (!GridFragment) return nullptr;


	const FIntPoint GridDimensions = GridFragment->GetGridSize();
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
	const FVector2D DrawSize = GridDimensions * IconTileWidth;
	//Calculate Draw Size

	//Create equipped slotted item widget

	//Set Slotted item's inventory item

	//Set Slotted item equipment type tag

	//Hide Stack count

	//Set Inventory Item on this class

	//Set Image Brush on slotted item

	//Add slotted item item to child to this widget overlay
	//return slotted item
	return nullptr;
}
