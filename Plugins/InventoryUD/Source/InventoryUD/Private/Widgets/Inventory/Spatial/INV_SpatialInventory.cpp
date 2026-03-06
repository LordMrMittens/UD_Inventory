// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/INV_SpatialInventory.h"
#include "Widgets/ItemDescription/INV_ItemDescription.h"
#include "Widgets/Inventory/GridSlots/INV_EquippedGridSlot.h"
#include "Widgets/SlottedItems/INV_EquippedSlottedItem.h"
#include "Widgets/Inventory/HoverItem/INV_HoverItem.h"
#include "InventoryManagement/Components/INV_InventoryComponent.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "InventoryUD.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Items/Components/INV_ItemComponent.h"
#include "Items/INV_InventoryItem.h"
#include "Items/Manifest/INV_ItemManifest.h"
#include "InventoryManagement/Utils/INV_InventoryStatics.h"
#include "Widgets/Inventory/Spatial/INV_InventoryGrid.h"

void UINV_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Equippables->OnClicked.AddDynamic(this, &UINV_SpatialInventory::ShowEquippables);
	Button_Consumables->OnClicked.AddDynamic(this, &UINV_SpatialInventory::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this, &UINV_SpatialInventory::ShowCraftables);

	GridEquippables->SetOwningCanvasPanel(CanvasPanel);
	GridConsumables->SetOwningCanvasPanel(CanvasPanel);
	GridCraftables->SetOwningCanvasPanel(CanvasPanel);

	ShowEquippables();
	WidgetTree->ForEachWidget([this](UWidget* Widget) {
		UINV_EquippedGridSlot* EquippedGridSlot = Cast<UINV_EquippedGridSlot>(Widget);
		if (IsValid(EquippedGridSlot)) {
			EquippedGridSlots.Add(EquippedGridSlot);
			EquippedGridSlot->EquippedGridSlotClicked.AddDynamic(this, &UINV_SpatialInventory::EquippedGridSlotClicked);
		}
		});
	
}

void UINV_SpatialInventory::EquippedGridSlotClicked(UINV_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTag)
{

	if (!CanEquipHoverItem(EquippedGridSlot, EquipmentTag)) return;
	const float TileSize = UINV_InventoryStatics::GetInventoryWidget(GetOwningPlayer())->GetTileSize();

	UINV_HoverItem* HoverItem = GetHoverItem();
	UINV_EquippedSlottedItem* EquippedSlottedItem = EquippedGridSlot->OnItemEquipped(
		HoverItem->GetInventoryItem(),
		EquipmentTag,
		TileSize
	);
	EquippedSlottedItem->OnSlottedItemClicked.AddDynamic(this, &ThisClass::EquippedSlottedItemClicked);

	//Tell server we are equipping something (do we need to unequip something as well? )

	UINV_InventoryComponent* InventoryComponent = UINV_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));
	InventoryComponent->Server_EquipSlotClicked(HoverItem->GetInventoryItem(), nullptr);

	if (GetOwningPlayer()->GetNetMode() != NM_DedicatedServer) {
		InventoryComponent->OnItemEquipped.Broadcast(HoverItem->GetInventoryItem());
	}
	GridEquippables->ClearHoverItem();
}

void UINV_SpatialInventory::EquippedSlottedItemClicked(UINV_EquippedSlottedItem* EquippedSlottedItem)
{
	UINV_InventoryStatics::ItemUnhovered(GetOwningPlayer());
	if (IsValid(GetHoverItem()) && GetHoverItem()->GetIsStackable()) return;

	UINV_InventoryItem* ItemToEquip = IsValid(GetHoverItem()) ? GetHoverItem()->GetInventoryItem() : nullptr;
	UINV_InventoryItem* ItemToUnequip = EquippedSlottedItem->GetInventoryItem();
	UINV_EquippedGridSlot* EquippedGridSlot = FindSlotWithEquippedItem(ItemToUnequip);
	GridEquippables->AssignHoverItem(ItemToUnequip);
	ClearSlotOfItem(EquippedGridSlot);
	RemoveEquippedSlottedItem(EquippedSlottedItem);
	MakeEquippedSlottedItem(EquippedSlottedItem, EquippedGridSlot, ItemToEquip);
	//broadcast any delegates needed
}

void UINV_SpatialInventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(ItemDescription)) return;

	SetItemDescriptionSizeAndPosition(ItemDescription, CanvasPanel);
}

void UINV_SpatialInventory::SetItemDescriptionSizeAndPosition(UINV_ItemDescription* Description, UCanvasPanel* Canvas) const
{
	UCanvasPanelSlot* ItemDescriptionCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(Description);
	if (!IsValid(ItemDescriptionCPS)) return;

	const FVector2D ItemDescriptionSize = Description->GetBoxSize();
	ItemDescriptionCPS->SetSize(ItemDescriptionSize);

	FVector2D ClampedPosition = UINV_WidgetUtils::GetClampedWidgetPosition(
		UINV_WidgetUtils::GetWidgetSize(Canvas), 
		ItemDescriptionSize, 
		UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer()));

	ItemDescriptionCPS->SetPosition(ClampedPosition);
}

FReply UINV_SpatialInventory::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	ActiveGrid->DropItem();
	
	return Super::NativeOnMouseButtonDown(MyGeometry, MouseEvent);
}

FINV_SlotAvailabilityResult UINV_SpatialInventory::HasRoomForItem(UINV_ItemComponent* ItemComponent) const
{
	switch (UINV_InventoryStatics::GetCategoryFromItemComponent(ItemComponent))
	{
	case EINV_ItemCategory::Equippable:
		return GridEquippables->HasRoomForItem(ItemComponent);
		break;
	case EINV_ItemCategory::Consumable:
		return GridConsumables->HasRoomForItem(ItemComponent);
		break;
	case EINV_ItemCategory::Craftable:
		return GridCraftables->HasRoomForItem(ItemComponent);
		break;
	default:
		UE_LOG(LogInventory, Error, TEXT("No Matching Item Category Was Found"));
		return FINV_SlotAvailabilityResult();
	}
}

void UINV_SpatialInventory::OnItemHovered(UINV_InventoryItem* Item)
{
	const auto& Manifest = Item->GetItemManifest();
	UINV_ItemDescription* DescriptionWidget = GetItemDescription();
	DescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(ItemDescriptionHandle);
	FTimerDelegate DescriptionTimerDelegate;
	DescriptionTimerDelegate.BindLambda([this, &Manifest, DescriptionWidget]() {

		Manifest.AssimilateInventoryFragments(DescriptionWidget);
		GetItemDescription()->SetVisibility(ESlateVisibility::HitTestInvisible);
		//Assimilate manifest indo item description
		});
	GetOwningPlayer()->GetWorldTimerManager().SetTimer(ItemDescriptionHandle, DescriptionTimerDelegate, ItemDescriptionDelay, false);
}

void UINV_SpatialInventory::OnItemUnhovered()
{
	UINV_ItemDescription* DescriptionWidget = GetItemDescription();
	DescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(ItemDescriptionHandle);
}

bool UINV_SpatialInventory::HasHoverItem() const
{
	if (GridEquippables->HasHoverItem()) return true;
	if (GridConsumables->HasHoverItem()) return true;
	if (GridCraftables->HasHoverItem()) return true;
	
	return false;
}

UINV_HoverItem* UINV_SpatialInventory::GetHoverItem() const
{

	if (!ActiveGrid.IsValid()) return nullptr;
	
	return ActiveGrid->GetHoverItem();
}

UINV_ItemDescription* UINV_SpatialInventory::GetItemDescription()
{
	if (!IsValid(ItemDescription)) {
		ItemDescription = CreateWidget<UINV_ItemDescription>(GetOwningPlayer(), ItemDescriptionClass);
		CanvasPanel->AddChild(ItemDescription);
	}
	
	return ItemDescription;
}

float UINV_SpatialInventory::GetTileSize() const
{

	return GridEquippables->GetTileSize();
}



void UINV_SpatialInventory::ShowEquippables()
{
	SetActiveGrid(GridEquippables, Button_Equippables);
}

void UINV_SpatialInventory::ShowConsumables()
{
	SetActiveGrid(GridConsumables, Button_Consumables);
}

void UINV_SpatialInventory::ShowCraftables()
{
	SetActiveGrid(GridCraftables, Button_Craftables);
}



void UINV_SpatialInventory::SetActiveGrid(UINV_InventoryGrid* Grid, UButton* Button)
{
	if (!IsValid(Switcher) || !Grid || !Button) return;
	if(ActiveGrid.IsValid()) ActiveGrid->HideCursor();
	ActiveGrid = Grid;
	if (ActiveGrid.IsValid()) ActiveGrid->ShowCursor();
	DisableButton(Button);
	Switcher->SetActiveWidget(Grid);
}

void UINV_SpatialInventory::DisableButton(UButton* Button)
{
	Button_Equippables->SetIsEnabled(true);
	Button_Consumables->SetIsEnabled(true);
	Button_Craftables->SetIsEnabled(true);
	Button->SetIsEnabled(false);
}

bool UINV_SpatialInventory::CanEquipHoverItem(UINV_EquippedGridSlot* EquipGridSlot, const FGameplayTag& EquipmentTypeTag) const
{
	if (!IsValid(EquipGridSlot) || EquipGridSlot->GetInventoryItem().IsValid()) return false;
	UINV_HoverItem* HoverItem = GetHoverItem();
	if (!IsValid(HoverItem)) return false;

	UINV_InventoryItem* HeldItem = HoverItem->GetInventoryItem();

	return HasHoverItem() && IsValid(HeldItem) && !HoverItem->GetIsStackable() && HeldItem->GetItemManifest().GetItemCategory() == EINV_ItemCategory::Equippable;
}

UINV_EquippedGridSlot* UINV_SpatialInventory::FindSlotWithEquippedItem(UINV_InventoryItem* EquippedItem) const
{
	auto* FoundEquippedGridSlot = EquippedGridSlots.FindByPredicate([EquippedItem](UINV_EquippedGridSlot* GridSlot) {

		return GridSlot->GetInventoryItem() == EquippedItem;
		});
	return FoundEquippedGridSlot ? *FoundEquippedGridSlot : nullptr;
}

void UINV_SpatialInventory::ClearSlotOfItem(UINV_EquippedGridSlot* EquippedGridSlot)
{
	if (!IsValid(EquippedGridSlot)) return;
	EquippedGridSlot->SetEquippedSlottedItem(nullptr);
	EquippedGridSlot->SetInventoryItem(nullptr);
}

void UINV_SpatialInventory::RemoveEquippedSlottedItem(UINV_EquippedSlottedItem* EquippedSlottedItem)
{
	if (!IsValid(EquippedSlottedItem)) return;

	if (EquippedSlottedItem->OnSlottedItemClicked.IsAlreadyBound(this, &ThisClass::EquippedSlottedItemClicked)) {
		EquippedSlottedItem->OnSlottedItemClicked.RemoveDynamic(this, &ThisClass::EquippedSlottedItemClicked);
		EquippedSlottedItem->RemoveFromParent();
	}

}

void UINV_SpatialInventory::MakeEquippedSlottedItem(UINV_EquippedSlottedItem* EquippedSlottedItem, UINV_EquippedGridSlot* EquippedGridSlot, UINV_InventoryItem* ItemToEquip)
{
	if (!IsValid(EquippedGridSlot)) return;

	UINV_EquippedSlottedItem* SlottedItem = EquippedGridSlot->OnItemEquipped(ItemToEquip, EquippedSlottedItem->GetEquipmentTypeTag(), UINV_InventoryStatics::GetInventoryWidget(GetOwningPlayer())->GetTileSize() );
	SlottedItem->OnSlottedItemClicked.AddDynamic(this, &ThisClass::EquippedSlottedItemClicked);
}
