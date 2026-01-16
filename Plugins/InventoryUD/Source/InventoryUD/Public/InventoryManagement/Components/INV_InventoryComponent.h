// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "INV_InventoryComponent.generated.h"

class UINV_InventoryBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class INVENTORYUD_API UINV_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UINV_InventoryComponent();

	UFUNCTION(BlueprintCallable)
	void ToggleInventoryMenu();

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
