// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SlottedItems/INV_SlottedItem.h"
#include "GameplayTagContainer.h"
#include "INV_EquippedSlottedItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquippedSlottedItemClicked, class UINV_EquippedSlottedItem*, EquippedSlottedItem);
/**
 * 
 */
UCLASS()
class INVENTORYUD_API UINV_EquippedSlottedItem : public UINV_SlottedItem
{
	GENERATED_BODY()
public:
	FEquippedSlottedItemClicked OnSlottedItemClicked;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY()
	FGameplayTag EquipmentTypeTag;
public:

	void SetEquipmentTypeTag(const FGameplayTag& Tag) { EquipmentTypeTag = Tag; }
	FGameplayTag GetEquipmentTypeTag() const { return EquipmentTypeTag; }
};
