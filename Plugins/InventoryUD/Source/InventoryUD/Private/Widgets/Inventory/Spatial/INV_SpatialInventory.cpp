// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/INV_SpatialInventory.h"
#include "Widgets/ItemDescription/INV_ItemDescription.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
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

	GridEquippables->SetOwningCanvasPanel(CanvasPanel);
	GridConsumables->SetOwningCanvasPanel(CanvasPanel);
	GridCraftables->SetOwningCanvasPanel(CanvasPanel);

	ShowEquippables();
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
	UINV_ItemDescription* DescriptionWidget = GetItemDescription();
	DescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(ItemDescriptionHandle);
	FTimerDelegate DescriptionTimerDelegate;
	DescriptionTimerDelegate.BindLambda([this]() {

		GetItemDescription()->SetVisibility(ESlateVisibility::HitTestInvisible);
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

UINV_ItemDescription* UINV_SpatialInventory::GetItemDescription()
{
	if (!IsValid(ItemDescription)) {
		ItemDescription = CreateWidget<UINV_ItemDescription>(GetOwningPlayer(), ItemDescriptionClass);
		CanvasPanel->AddChild(ItemDescription);
	}
	
	return ItemDescription;
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
