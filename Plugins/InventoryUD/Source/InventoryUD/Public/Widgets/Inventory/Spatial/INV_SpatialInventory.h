// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Inventory/InventoryBase/INV_InventoryBase.h"
#include "INV_SpatialInventory.generated.h"

class UINV_InventoryGrid;
class UWidgetSwitcher;
class UButton;
/**
 * 
 */
UCLASS()
class INVENTORYUD_API UINV_SpatialInventory : public UINV_InventoryBase
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UINV_InventoryGrid> GridEquippables;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UINV_InventoryGrid> GridConsumables;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UINV_InventoryGrid> GridCraftables;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Equippables;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Consumables;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Craftables;

	UFUNCTION()
	void ShowEquippables();
	UFUNCTION()
	void ShowConsumables();	
	UFUNCTION()
	void ShowCraftables();

	void SetActiveGrid(UINV_InventoryGrid* Grid, UButton* Button);
	void DisableButton(UButton* Button);

};
