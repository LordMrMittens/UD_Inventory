// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentManagement/Components/INV_EquipmentComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Items/INV_InventoryItem.h"
#include "Items/Manifest/INV_ItemManifest.h"
#include "Items/Fragments/INV_ItemFragment.h"
#include "InventoryManagement/Components/INV_InventoryComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InventoryManagement/Utils/INV_InventoryStatics.h"

// Called when the game starts
void UINV_EquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningPlayerController = Cast<APlayerController>(GetOwner());
	if (OwningPlayerController.IsValid()) 
	{
		ACharacter* OwningCharacter = Cast<ACharacter>(OwningPlayerController->GetPawn());
		if (IsValid(OwningCharacter)) {
			OwningSkeletalMesh = OwningCharacter->GetMesh();
		}
		InitInventoryComponent();
	}
}

void UINV_EquipmentComponent::InitInventoryComponent()
{
	InventoryComponent = UINV_InventoryStatics::GetInventoryComponent(OwningPlayerController.Get());
	if (!InventoryComponent.IsValid()) return;
	if (!InventoryComponent->OnItemEquipped.IsAlreadyBound(this, &ThisClass::OnItemEquipped)) {
		InventoryComponent->OnItemEquipped.AddDynamic(this, &ThisClass::OnItemEquipped);
	}
	if (!InventoryComponent->OnItemUnequipped.IsAlreadyBound(this, &ThisClass::OnItemUnequipped)) {
		InventoryComponent->OnItemUnequipped.AddDynamic(this, &ThisClass::OnItemUnequipped);
	}

}
void UINV_EquipmentComponent::OnItemEquipped(UINV_InventoryItem* EquippedItem)
{
	if (!IsValid(EquippedItem)) return;
	if (!OwningPlayerController->HasAuthority()) return;

	FINV_ItemManifest& ItemManifest = EquippedItem->GetItemManifestMutable();
	FINV_EquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FINV_EquipmentFragment>();
	if (!EquipmentFragment) return;

	EquipmentFragment->OnEquip(OwningPlayerController.Get());
}

void UINV_EquipmentComponent::OnItemUnequipped(UINV_InventoryItem* UnequippedItem)
{
	if (!IsValid(UnequippedItem)) return;
	if (!OwningPlayerController->HasAuthority()) return;

	FINV_ItemManifest& ItemManifest = UnequippedItem->GetItemManifestMutable();
	FINV_EquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FINV_EquipmentFragment>();
	if (!EquipmentFragment) return;

	EquipmentFragment->OnUnequip(OwningPlayerController.Get());
}




