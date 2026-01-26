// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Components/INV_InventoryComponent.h"
#include "Widgets/Inventory/InventoryBase/INV_InventoryBase.h"

UINV_InventoryComponent::UINV_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UINV_InventoryComponent::TryAddItem(UINV_ItemComponent* ItemComponent)
{
	OnNoRoomInInventory.Broadcast();
}


// Called when the game starts
void UINV_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	ConstructInventory();

	
}

void UINV_InventoryComponent::ConstructInventory()
{
	OwningController = Cast<APlayerController>(GetOwner());
	checkf(OwningController.IsValid(), TEXT("Inventory Component must have a Player Controller as Owner."));
	if (!OwningController->IsLocalController()) return;
	InventoryMenu = CreateWidget<UINV_InventoryBase>(OwningController.Get(), InventoryMenuClass);
	InventoryMenu->AddToViewport();
	CloseInventoryMenu();
}

void UINV_InventoryComponent::ToggleInventoryMenu()
{
	bInventoryMenuOpen ? CloseInventoryMenu() : OpenInventoryMenu();
}

void UINV_InventoryComponent::OpenInventoryMenu()
{
	if(!IsValid(InventoryMenu)) return;
	InventoryMenu->SetVisibility(ESlateVisibility::Visible);
	bInventoryMenuOpen = true;
	if (!OwningController.IsValid()) return;
	FInputModeGameAndUI InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(true);

}

void UINV_InventoryComponent::CloseInventoryMenu()
{
	if (!IsValid(InventoryMenu)) return;
	InventoryMenu->SetVisibility(ESlateVisibility::Collapsed);
	bInventoryMenuOpen = false;
	if (!OwningController.IsValid()) return;
	FInputModeGameOnly InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(false);

}

