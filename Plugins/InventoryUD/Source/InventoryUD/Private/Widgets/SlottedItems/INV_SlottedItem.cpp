// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SlottedItems/INV_SlottedItem.h"
#include "Items/INV_InventoryItem.h"
#include "Components/Image.h"

void UINV_SlottedItem::SetImageBrush(const FSlateBrush& Brush) const
{
	Image_Icon->SetBrush(Brush);
}

void UINV_SlottedItem::SetInventoryItem(UINV_InventoryItem* Item)
{
	{ InventoryItem = Item; }
}
