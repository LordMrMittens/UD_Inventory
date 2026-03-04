// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Inventory/InventoryBase/INV_InventoryBase.h"
#include "INV_SpatialInventory.generated.h"

class UINV_InventoryGrid;
class UINV_ItemDescription;
class UINV_EquippedGridSlot;
class UINV_HoverItem;
class UWidgetSwitcher;
class UButton;
class UCanvasPanel;
class FGameplayTag;
/**
 * 
 */
UCLASS()
class INVENTORYUD_API UINV_SpatialInventory : public UINV_InventoryBase
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual FINV_SlotAvailabilityResult HasRoomForItem(UINV_ItemComponent* ItemComponent) const override;

	virtual void OnItemHovered(UINV_InventoryItem* Item) override;

	virtual void OnItemUnhovered() override;

	virtual bool HasHoverItem() const override;
	
	virtual UINV_HoverItem* GetHoverItem() const override;

	UINV_ItemDescription* GetItemDescription();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

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

	TWeakObjectPtr<UINV_InventoryGrid> ActiveGrid;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UINV_ItemDescription> ItemDescriptionClass;

	UPROPERTY()
	TObjectPtr<UINV_ItemDescription> ItemDescription;


	UPROPERTY(EditAnywhere, Category = "Inventory")
	float ItemDescriptionDelay{ 0.5f };

	UPROPERTY()
	TArray<TObjectPtr<UINV_EquippedGridSlot>> EquippedGridSlots;

	FTimerHandle ItemDescriptionHandle;

	void SetItemDescriptionSizeAndPosition(UINV_ItemDescription* Description, UCanvasPanel* Canvas)const;

	UFUNCTION()
	void ShowEquippables();
	UFUNCTION()
	void ShowConsumables();	
	UFUNCTION()
	void ShowCraftables();
	UFUNCTION()
	void EquippedGridSlotClicked(UINV_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTag);

	void SetActiveGrid(UINV_InventoryGrid* Grid, UButton* Button);
	void DisableButton(UButton* Button);
	bool CanEquipHoverItem(UINV_EquippedGridSlot* EquipGridSlot, const FGameplayTag& EquipmentTypeTag ) const;

};
