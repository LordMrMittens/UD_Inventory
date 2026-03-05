// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Components/INV_InventoryComponent.h"
#include "Widgets/Inventory/InventoryBase/INV_InventoryBase.h"
#include "Items/Components/INV_ItemComponent.h"
#include "Items/INV_InventoryItem.h"
#include "Items/Fragments/INV_ItemFragment.h"
#include "Net/UnrealNetwork.h"

UINV_InventoryComponent::UINV_InventoryComponent() : InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
	bInventoryMenuOpen = false;
	// ...
}

void UINV_InventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, InventoryList);
}

void UINV_InventoryComponent::TryAddItem(UINV_ItemComponent* ItemComponent)
{
	FINV_SlotAvailabilityResult Result = InventoryMenu->HasRoomForItem(ItemComponent);
	
	UINV_InventoryItem* FoundItem = InventoryList.FindItemByType(ItemComponent->GetItemManifest().GetItemType());
	Result.Item = FoundItem;
	
	if(Result.TotalRoomToFill ==0)
	{
		OnNoRoomInInventory.Broadcast();
		return;
	}

	if (Result.Item.IsValid() && Result.bStackable) {
		OnStackChange.Broadcast(Result);
		Server_AddStacksToItem(ItemComponent, Result.TotalRoomToFill, Result.Remainder);
	}
	else if (Result.TotalRoomToFill > 0)
	{
		Server_AddNewItem(ItemComponent, Result.bStackable ? Result.TotalRoomToFill : 0, Result.bStackable ? Result.Remainder : 0);
		//this item doesnt exist in the inventory, creat a new one and update slots
	}
}
void UINV_InventoryComponent::Server_AddNewItem_Implementation(UINV_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder)
{

    UINV_InventoryItem* NewItem = InventoryList.AddEntry(ItemComponent);
	NewItem->SetTotalStackCount(StackCount);
	if (GetOwner()->GetNetMode() == NM_ListenServer || GetOwner()->GetNetMode() == NM_Standalone) {
		OnItemAdded.Broadcast(NewItem);
	}
	PickupOrUpdateStack(Remainder, ItemComponent);
}


void UINV_InventoryComponent::Server_AddStacksToItem_Implementation(UINV_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder)
{
	const FGameplayTag& ItemType = IsValid(ItemComponent) ?  ItemComponent->GetItemManifest().GetItemType() : FGameplayTag::EmptyTag;
	UINV_InventoryItem* Item = InventoryList.FindItemByType(ItemType);
	if (!IsValid(Item)) return;
	Item->SetTotalStackCount(Item->GetTotalStackCount() + StackCount);
	PickupOrUpdateStack(Remainder, ItemComponent);
}

void UINV_InventoryComponent::PickupOrUpdateStack(int32 Remainder, UINV_ItemComponent* ItemComponent)
{
	if (Remainder == 0) {
		ItemComponent->PickedUp();
	}
	else if (FINV_StackableFragment* StackableFragment = ItemComponent->GetItemManifest().GetFragmentOfTypeMutable<FINV_StackableFragment>())
	{
		StackableFragment->SetStackCount(Remainder);
	}
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

void UINV_InventoryComponent::Server_DropItem_Implementation(UINV_InventoryItem* Item, int32 StackCount)
{
	const int32 NewStackCount = Item->GetTotalStackCount() - StackCount;
	if (NewStackCount <= 0) 
	{
		InventoryList.RemoveEntry(Item);
	}
	else 
	{
		Item->SetTotalStackCount(NewStackCount);
	}
	SpawnDroppedItem(Item, StackCount);
}

void UINV_InventoryComponent::SpawnDroppedItem(UINV_InventoryItem* Item, int32 StackCount)
{
	const APawn* OwningPawn = OwningController->GetPawn();
	FVector RotatedForward = OwningPawn->GetActorForwardVector();
	RotatedForward = RotatedForward.RotateAngleAxis(FMath::RandRange(DropSpawnAngleMin, DropSpawnAngleMax), FVector::UpVector);
	FVector SpawnLocation = OwningPawn->GetActorLocation() + RotatedForward * (FMath::RandRange(DropSpawnDistanceMin, DropSpawnDistanceMax));
	SpawnLocation.Z -= RelativeSpawnElevation;
	const FRotator SpawnRotation = FRotator::ZeroRotator;

	FINV_ItemManifest& ItemManifest = Item->GetItemManifestMutable();
	if (FINV_StackableFragment* StackableFragment = ItemManifest.GetFragmentOfTypeMutable<FINV_StackableFragment>())
	{
		StackableFragment->SetStackCount(StackCount);
	}
	ItemManifest.SpawnPickupActor(this, SpawnLocation, SpawnRotation);
}

void UINV_InventoryComponent::Server_ConsumeItem_Implementation(UINV_InventoryItem* Item)
{
	const int32 NewStackCount = Item->GetTotalStackCount() - 1;
	if (NewStackCount <= 0) {
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		Item->SetTotalStackCount(NewStackCount);
	}

	if (FINV_ConsumableFragment* ConsumableFragment = Item->GetItemManifestMutable().GetFragmentOfTypeMutable<FINV_ConsumableFragment>()) {
		ConsumableFragment->OnConsume(OwningController.Get());
	}

}

void UINV_InventoryComponent::Server_EquipSlotClicked_Implementation(UINV_InventoryItem* ItemToEquip, UINV_InventoryItem* ItemToUnequip)
{
	Multicast_EquipSlotClicked(ItemToEquip, ItemToUnequip);

}

void UINV_InventoryComponent::Multicast_EquipSlotClicked_Implementation(UINV_InventoryItem* ItemToEquip, UINV_InventoryItem* ItemToUnequip)
{
	OnItemEquipped.Broadcast(ItemToEquip);
	OnItemUnequipped.Broadcast(ItemToUnequip);
}

void UINV_InventoryComponent::ToggleInventoryMenu()
{
	bInventoryMenuOpen ? CloseInventoryMenu() : OpenInventoryMenu();
}

void UINV_InventoryComponent::AddRepSubObject(UObject* SubObj)
{
	if(IsUsingRegisteredSubObjectList() && IsReadyForReplication() && IsValid(SubObj))
	{
		AddReplicatedSubObject(SubObj);
	}
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

