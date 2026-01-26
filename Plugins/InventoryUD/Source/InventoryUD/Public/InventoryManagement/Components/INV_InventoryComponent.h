// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "INV_InventoryComponent.generated.h"


class UINV_InventoryBase;
class UINV_InventoryItem;
class UINV_ItemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemChange, UINV_InventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoRoomInInventory);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class INVENTORYUD_API UINV_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UINV_InventoryComponent();

	UFUNCTION(BlueprintCallable,BlueprintAuthorityOnly ,Category = "Inventory")
	void TryAddItem(UINV_ItemComponent* ItemComponent);

	UFUNCTION(Server,Reliable)
	void Server_AddNewItem(UINV_ItemComponent* ItemComponent, int32 StackCount);

	UFUNCTION(Server, Reliable)
	void Server_AddStacksToItem(UINV_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder);

	UFUNCTION(BlueprintCallable)
	void ToggleInventoryMenu();


	FInventoryItemChange OnItemAdded;
	FInventoryItemChange OnItemRemoved; 
	FNoRoomInInventory OnNoRoomInInventory;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:	


	void ConstructInventory();
	void OpenInventoryMenu();
	void CloseInventoryMenu();

	TWeakObjectPtr<APlayerController> OwningController;
	UPROPERTY()
	TObjectPtr<UINV_InventoryBase> InventoryMenu;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UINV_InventoryBase> InventoryMenuClass;

	bool bInventoryMenuOpen;
	
		
};
