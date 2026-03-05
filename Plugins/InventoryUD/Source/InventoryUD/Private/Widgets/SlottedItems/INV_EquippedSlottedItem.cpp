// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SlottedItems/INV_EquippedSlottedItem.h"

FReply UINV_EquippedSlottedItem::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& InMouseEvent)
{
	OnSlottedItemClicked.Broadcast(this);
	return FReply::Handled();
}
