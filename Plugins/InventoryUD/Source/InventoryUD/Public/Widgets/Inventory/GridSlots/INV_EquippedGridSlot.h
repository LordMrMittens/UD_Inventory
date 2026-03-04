// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Inventory/GridSlots/INV_GridSlot.h"
#include "GameplayTagContainer.h"
#include "INV_EquippedGridSlot.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquippedGridSlotClicked, UINV_EquippedGridSlot*, GridSlot, const FGameplayTag&, EquipmentTagType);
/**
 * 
 */
UCLASS()
class INVENTORYUD_API UINV_EquippedGridSlot : public UINV_GridSlot
{
	GENERATED_BODY()
public: 
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	FEquippedGridSlotClicked EquippedGridSlotClicked;

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag EquipmentTypeTag;
};
