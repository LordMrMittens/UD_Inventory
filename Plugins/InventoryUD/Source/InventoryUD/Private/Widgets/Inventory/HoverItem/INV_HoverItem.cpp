


#include "Widgets/Inventory/HoverItem/INV_HoverItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Items/INV_InventoryItem.h"

void UINV_HoverItem::SetImageBrush(const FSlateBrush& Brush) const
{
	Image_Icon->SetBrush(Brush);
}

void UINV_HoverItem::UpdateStackCount(const int32 Count) const
{
	if (Count > 0) {

		Text_StackCount->SetText(FText::AsNumber(Count));
		Text_StackCount->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Hidden);
	}
}

FGameplayTag UINV_HoverItem::GetItemType() const
{
	if (InventoryItem.IsValid()) {
		return InventoryItem->GetItemManifest().GetItemType();
	}
	return FGameplayTag::EmptyTag;
}
void UINV_HoverItem::SetIsStackable(const bool bStackable) 
{
	bIsStackable = bStackable;
	bIsStackable ? Text_StackCount->SetVisibility(ESlateVisibility::Visible) : Text_StackCount->SetVisibility(ESlateVisibility::Hidden);

}

UINV_InventoryItem* UINV_HoverItem::GetInventoryItem() const
{
	return InventoryItem.Get();
}

void UINV_HoverItem::SetInventoryItem(UINV_InventoryItem* Item)
{
	InventoryItem = Item;
}