// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "INV_EquipmentComponent.generated.h"

class UINV_InventoryComponent;
class UINV_InventoryItem;
class AINV_EquipActor;
class USkeletalMeshComponent;
struct FINV_EquipmentFragment;
struct FINV_ItemManifest;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class INVENTORYUD_API UINV_EquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:	
	
	TWeakObjectPtr<UINV_InventoryComponent> InventoryComponent;
	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<USkeletalMeshComponent> OwningSkeletalMesh;

	UFUNCTION()
	void OnItemEquipped(UINV_InventoryItem* EquippedItem);
	UFUNCTION()
	void OnItemUnequipped(UINV_InventoryItem* EquippedItem);

	void InitInventoryComponent();

	AINV_EquipActor* SpawnEquippedActor(FINV_EquipmentFragment* EquipmentFragment, const FINV_ItemManifest& Manifest, USkeletalMeshComponent* AttachMesh);
	UPROPERTY()
	TArray<TObjectPtr<AINV_EquipActor>> EquippedActors;

	AINV_EquipActor* FindEquippedActorByTag(const FGameplayTag& EquipmentTypeTag);
	void RemoveEquippedActor(const FGameplayTag& EquipmentTypeTag);
};
