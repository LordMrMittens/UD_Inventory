// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "INV_GridSlot.generated.h"

class UImage;
class UINV_InventoryItem;
class UINV_ItemPopUp;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGridSlotEvent, int32, GridIndex, const FPointerEvent&, MouseEvent);

UENUM(BlueprintType)
enum class EINV_GridSlotState : uint8
{
	Unoccupied,
	Occupied,
	Selected,
	GreyedOut
};


UCLASS()
class INVENTORYUD_API UINV_GridSlot : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;


	void SetUnoccupiedTexture();
	void SetOccupiedTexture();
	void SetSelectedTexture();
	void SetGreyedOutTexture();
	void SetInventoryItem(UINV_InventoryItem* Item);
	UINV_ItemPopUp* GetItemPopUp() const;
	void SetItemPopUp(UINV_ItemPopUp* PopUp);

	FGridSlotEvent GridSlotClicked;
	FGridSlotEvent GridSlotHovered;
	FGridSlotEvent GridSlotUnhovered;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;

	int32 TileIndex{INDEX_NONE};
	int32 UpperLeftIndex{INDEX_NONE};
	int32 StackCount{0};

	bool bIsAvailable{true};

	TWeakObjectPtr<UINV_InventoryItem> InventoryItem;
	TWeakObjectPtr<UINV_ItemPopUp> ItemPopUp;

	EINV_GridSlotState GridSlotState = EINV_GridSlotState::Occupied;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Unoccupied;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Occupied;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Selected;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_GreyedOut;


	UFUNCTION()
	void OnItemPopUpDestruct(UUserWidget* Menu);

public:
	FORCEINLINE void SetTileIndex(int32 Index) { TileIndex = Index; }
	FORCEINLINE int32 GetTileIndex() const { return TileIndex; }
	FORCEINLINE EINV_GridSlotState GetGridSlotState() { return GridSlotState; }
	FORCEINLINE TWeakObjectPtr<UINV_InventoryItem> GetInventoryItem() const { return InventoryItem; }
	FORCEINLINE int32 GetStackCount() const { return StackCount; }
	FORCEINLINE void SetStackCount(int32 Count) { StackCount = Count; }
	FORCEINLINE int32 GetUpperLeftIndex() const { return UpperLeftIndex; }
	FORCEINLINE void SetUpperLeftIndex(int32 Index) { UpperLeftIndex = Index; }
	FORCEINLINE bool GetIsAvailable() const { return bIsAvailable; }
	FORCEINLINE void SetIsAvailable(bool bAvailable) { bIsAvailable = bAvailable; }

};
