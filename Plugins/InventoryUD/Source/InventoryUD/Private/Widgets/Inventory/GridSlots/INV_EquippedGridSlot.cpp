// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/GridSlots/INV_EquippedGridSlot.h"
#include "InventoryManagement/Utils/INV_InventoryStatics.h"
#include "Widgets/Inventory/HoverItem/INV_HoverItem.h"

void UINV_EquippedGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	if (!GetIsAvailable()) return;
	UINV_HoverItem* HoverItem = UINV_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem)) return;

	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		SetOccupiedTexture();
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
	}
}

FReply UINV_EquippedGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	EquippedGridSlotClicked.Broadcast(this, EquipmentTypeTag);
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
