// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "INV_GridSlot.generated.h"

class UImage;

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
	void SetUnoccupiedTexture();
	void SetOccupiedTexture();
	void SetSelectedTexture();
	void SetGreyedOutTexture();

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;

	int32 TileIndex;

	EINV_GridSlotState GridSlotState = EINV_GridSlotState::Occupied;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Unoccupied;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Occupied;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Selected;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_GreyedOut;




public:
	FORCEINLINE void SetTileIndex(int32 Index) { TileIndex = Index; }
	FORCEINLINE int32 GetTileIndex() { return TileIndex; }
	FORCEINLINE EINV_GridSlotState GetGridSlotState() { return GridSlotState; }
};
