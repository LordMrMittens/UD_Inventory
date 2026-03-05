// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/GridSlots/INV_EquippedGridSlot.h"
#include "InventoryManagement/Utils/INV_InventoryStatics.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Items/INV_InventoryItem.h"
#include "Items/Fragments/INV_ItemFragment.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Widgets/SlottedItems/INV_EquippedSlottedItem.h"
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

	EquippedSlottedItem = CreateWidget<UINV_EquippedSlottedItem>(GetOwningPlayer(), EquippedSlottedItemClass);

	EquippedSlottedItem->SetInventoryItem(Item);

	EquippedSlottedItem->SetEquipmentTypeTag(EquipmentTag);

	EquippedSlottedItem->UpdateStackCount(0);
	
	SetInventoryItem(Item);

	const FINV_ImageFragment* ImageFragment = GetFragment<FINV_ImageFragment>(Item, Fragments::Icon);
	if (!ImageFragment) return nullptr;
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = DrawSize;
	EquippedSlottedItem->SetImageBrush(Brush);
	Overlay_Root->AddChildToOverlay(EquippedSlottedItem);
	FGeometry OverlayGeometry = Overlay_Root->GetCachedGeometry();
	auto OverlayPos = OverlayGeometry.Position;
	auto OverlaySize = OverlayGeometry.Size;

	const float LeftPadding = OverlaySize.X / 2.f - DrawSize.X / 2.f;
	const float TopPadding = OverlaySize.Y / 2.f - DrawSize.Y / 2.f;

	UOverlaySlot* OverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(EquippedSlottedItem);
	OverlaySlot->SetPadding(FMargin(LeftPadding, TopPadding));
	//return slotted item
	return EquippedSlottedItem;
}
