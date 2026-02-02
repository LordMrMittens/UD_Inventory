// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/GridSlots/INV_GridSlot.h"
#include "Items/INV_InventoryItem.h"
#include "Components/Image.h"


void UINV_GridSlot::SetUnoccupiedTexture()
{
	GridSlotState = EINV_GridSlotState::Unoccupied;
	Image_GridSlot->SetBrush(Brush_Unoccupied);
}

void UINV_GridSlot::SetOccupiedTexture()
{
	GridSlotState = EINV_GridSlotState::Occupied;
	Image_GridSlot->SetBrush(Brush_Occupied);
}

void UINV_GridSlot::SetSelectedTexture()
{
	GridSlotState = EINV_GridSlotState::Selected;
	Image_GridSlot->SetBrush(Brush_Selected);
}

void UINV_GridSlot::SetGreyedOutTexture()
{
	GridSlotState = EINV_GridSlotState::GreyedOut;
	Image_GridSlot->SetBrush(Brush_GreyedOut);
}
void UINV_GridSlot::SetInventoryItem(UINV_InventoryItem* Item)
{
	{ InventoryItem = Item; }
}