// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryManagement\FastArray\INV_FastArray.h"
#include "INV_InventoryComponent.generated.h"


class UINV_InventoryBase;
class UINV_InventoryItem;
class UINV_ItemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemChange, UINV_InventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoRoomInInventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStackChange, const FINV_SlotAvailabilityResult&, Result);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class INVENTORYUD_API UINV_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UINV_InventoryComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable,BlueprintAuthorityOnly ,Category = "Inventory")
	void TryAddItem(UINV_ItemComponent* ItemComponent);

	UFUNCTION(Server,Reliable)
	void Server_AddNewItem(UINV_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder);

	UFUNCTION(Server, Reliable)
	void Server_AddStacksToItem(UINV_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder);

	UFUNCTION(Server, Reliable)
	void Server_DropItem(UINV_InventoryItem* Item, int32 StackCount);

	UFUNCTION(Server, Reliable)
	void Server_ConsumeItem(UINV_InventoryItem* Item);

	UFUNCTION(BlueprintCallable)
	void ToggleInventoryMenu();

	void AddRepSubObject(UObject* SubObj);

	void SpawnDroppedItem(UINV_InventoryItem* Item, int32 StackCount);



	FInventoryItemChange OnItemAdded;
	FInventoryItemChange OnItemRemoved; 
	FNoRoomInInventory OnNoRoomInInventory;
	FStackChange OnStackChange;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:	


	void ConstructInventory();

	UPROPERTY(Replicated)
	FINV_InventoryFastArray InventoryList;

	void OpenInventoryMenu();
	void CloseInventoryMenu();
	void PickupOrUpdateStack(int32 Remainder, UINV_ItemComponent* ItemComponent);

	TWeakObjectPtr<APlayerController> OwningController;
	UPROPERTY()
	TObjectPtr<UINV_InventoryBase> InventoryMenu;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UINV_InventoryBase> InventoryMenuClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnAngleMin{ -85.0f };
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnAngleMax{ 85.0f };
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnDistanceMin{ 15.0f };
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnDistanceMax{ 50.0f };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float RelativeSpawnElevation{ 70.0f };
	bool bInventoryMenuOpen;
	
public:

	UINV_InventoryBase* GetInventoryMenu() const { return InventoryMenu; }

};
