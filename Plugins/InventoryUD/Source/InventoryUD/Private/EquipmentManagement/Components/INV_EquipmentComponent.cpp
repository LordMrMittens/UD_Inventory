// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentManagement/Components/INV_EquipmentComponent.h"
#include "EquipmentManagement/INV_EquipActor.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Items/INV_InventoryItem.h"
#include "Items/Manifest/INV_ItemManifest.h"
#include "Items/Manifest/INV_ItemManifest.h"
#include "Items/Fragments/INV_ItemFragment.h"
#include "InventoryManagement/Components/INV_InventoryComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InventoryManagement/Utils/INV_InventoryStatics.h"

// Called when the game starts
void UINV_EquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	InitPlayerController();
}

void UINV_EquipmentComponent::SetOwningSkeletalMesh(USkeletalMeshComponent* SkeletalMesh)
{
	OwningSkeletalMesh = SkeletalMesh;
}

void UINV_EquipmentComponent::InitializeOwner(APlayerController* PlayerController)
{
	if (IsValid(PlayerController)) {
		OwningPlayerController = PlayerController;
	}
	InitInventoryComponent();
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
void UINV_EquipmentComponent::InitPlayerController()
{
	if (OwningPlayerController = Cast<APlayerController>(GetOwner()); OwningPlayerController.IsValid())
	{
		if (ACharacter* OwningCharacter = Cast<ACharacter>(OwningPlayerController->GetPawn()); IsValid(OwningCharacter)) {
			OnPossesedPawnChanged(nullptr, OwningCharacter);
		}
		else
		{
			OwningPlayerController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossesedPawnChanged);
		}
		
	}
}

void UINV_EquipmentComponent::OnPossesedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (ACharacter* OwningCharacter = Cast<ACharacter>(OwningPlayerController->GetPawn()); IsValid(OwningCharacter)) {
		OwningSkeletalMesh = OwningCharacter->GetMesh();
	}
	InitInventoryComponent();
}
AINV_EquipActor* UINV_EquipmentComponent::SpawnEquippedActor(FINV_EquipmentFragment* EquipmentFragment, const FINV_ItemManifest& Manifest, USkeletalMeshComponent* AttachMesh)
{
	AINV_EquipActor* SpawnedEquipActor = EquipmentFragment->SpawnAttachedActor(AttachMesh);
	SpawnedEquipActor->SetEquipmentType(EquipmentFragment->GetEquipmentType());
	SpawnedEquipActor->SetOwner(GetOwner());
	EquipmentFragment->SetEquippedActor(SpawnedEquipActor);
	return SpawnedEquipActor;
}
AINV_EquipActor* UINV_EquipmentComponent::FindEquippedActorByTag(const FGameplayTag& EquipmentTypeTag)
{

	auto FoundActor = EquippedActors.FindByPredicate([&EquipmentTypeTag](const AINV_EquipActor* EquippedActor) {
		return EquippedActor->GetEquipmentType().MatchesTagExact(EquipmentTypeTag);
		});
	return FoundActor ? * FoundActor :nullptr;
}
void UINV_EquipmentComponent::RemoveEquippedActor(const FGameplayTag& EquipmentTypeTag)
{
	if (AINV_EquipActor* EquippedActor = FindEquippedActorByTag(EquipmentTypeTag)) {
		EquippedActors.Remove(EquippedActor);
		EquippedActor->Destroy();
	}
}

void UINV_EquipmentComponent::OnItemEquipped(UINV_InventoryItem* EquippedItem)
{
	if (!IsValid(EquippedItem)) return;
	if (!OwningPlayerController->HasAuthority()) return;

	FINV_ItemManifest& ItemManifest = EquippedItem->GetItemManifestMutable();
	FINV_EquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FINV_EquipmentFragment>();
	if (!EquipmentFragment) return;
	if(!bIsProxy)
	{
		EquipmentFragment->OnEquip(OwningPlayerController.Get());
	}

	if (!OwningSkeletalMesh.IsValid()) return;
	AINV_EquipActor* SpawnedEquippedActor = SpawnEquippedActor(EquipmentFragment, ItemManifest, OwningSkeletalMesh.Get());
	EquippedActors.Add(SpawnedEquippedActor);
}

void UINV_EquipmentComponent::OnItemUnequipped(UINV_InventoryItem* UnequippedItem)
{
	if (!IsValid(UnequippedItem)) return;
	if (!OwningPlayerController->HasAuthority()) return;

	FINV_ItemManifest& ItemManifest = UnequippedItem->GetItemManifestMutable();
	FINV_EquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FINV_EquipmentFragment>();
	if (!EquipmentFragment) return;
	if (!bIsProxy)
	{
		EquipmentFragment->OnUnequip(OwningPlayerController.Get());
	}
	RemoveEquippedActor(EquipmentFragment->GetEquipmentType());
}




