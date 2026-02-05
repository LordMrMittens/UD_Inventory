

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "INV_HoverItem.generated.h"

class UImage;
class UTextBlock;
class UINV_InventoryItem;

UCLASS(Blueprintable)
class INVENTORYUD_API UINV_HoverItem : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetImageBrush(const FSlateBrush& Brush) const;
	void UpdateStackCount(const int32 Count) const;
	void SetIsStackable(const bool bStackable);
	UINV_InventoryItem* GetInventoryItem() const;
	void SetInventoryItem(UINV_InventoryItem* Item);
	FGameplayTag GetItemType() const;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;

	int32 PreviousGridIndex;
	FIntPoint GridDimensions;
	TWeakObjectPtr<UINV_InventoryItem> InventoryItem;
	bool bIsStackable{ false };
	int32 StackCount{ 0 };

public:

	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(const int32 Count) { StackCount = Count; }
	int32 GetPreviousGridIndex() const { return PreviousGridIndex; }
	void SetPreviousGridIndex(const int32 Index) { PreviousGridIndex = Index; }
	FIntPoint GetGridDimensions() const { return GridDimensions; }
	void SetGridDimensions(const FIntPoint& Dimensions) { GridDimensions = Dimensions; }
	bool GetIsStackable() const { return bIsStackable; }
	

};
