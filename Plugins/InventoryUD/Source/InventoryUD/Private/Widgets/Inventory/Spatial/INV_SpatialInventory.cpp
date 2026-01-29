// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/INV_SpatialInventory.h"
#include "InventoryUD.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Items/Components/INV_ItemComponent.h"
#include "InventoryManagement/Utils/INV_InventoryStatics.h"
#include "Widgets/Inventory/Spatial/INV_InventoryGrid.h"

void UINV_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Equippables->OnClicked.AddDynamic(this, &UINV_SpatialInventory::ShowEquippables);
	Button_Consumables->OnClicked.AddDynamic(this, &UINV_SpatialInventory::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this, &UINV_SpatialInventory::ShowCraftables);
	ShowEquippables();
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
