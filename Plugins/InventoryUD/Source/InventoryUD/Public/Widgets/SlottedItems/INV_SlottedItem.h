// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "INV_SlottedItem.generated.h"

class UINV_InventoryItem;
class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSlottedItemClicked, int32, GridIndex, const FPointerEvent&, MouseEvent);
/**
 * 
 */
UCLASS()
class INVENTORYUD_API UINV_SlottedItem : public UUserWidget
{
	GENERATED_BODY()
public:

	FSlottedItemClicked OnSlottedItemClicked;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	void SetImageBrush(const FSlateBrush& Brush) const;
	void SetInventoryItem(UINV_InventoryItem* Item);
	void UpdateStackCount(int32 StackCount);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr <UTextBlock> Text_StackCount;

	int32 GridIndex;

	FIntPoint GridDimensions;

	TWeakObjectPtr<UINV_InventoryItem> InventoryItem;

	bool bIsStackable{ false };

public:

	void SetIsStackable( bool bStackable) { bIsStackable = bStackable; }
	bool GetIsStackable() const { return bIsStackable; }

	UImage* GetIcon() const { return Image_Icon; }

	void SetGridIndex( int32 Index) { GridIndex= Index; }
	int32 GetGridIndex() const { return GridIndex; }

	void SetGridDimensions(const FIntPoint Dimensions) { GridDimensions = Dimensions; }
	FIntPoint GetGridDimensions() const { return GridDimensions; }

	UINV_InventoryItem* GetInventoryItem() { return InventoryItem.Get(); }

};
