// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "INV_GridSlot.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class INVENTORYUD_API UINV_GridSlot : public UUserWidget
{
	GENERATED_BODY()
private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;

	int32 TileIndex;

public:
	FORCEINLINE void SetTileIndex(int32 Index) { TileIndex = Index; }
	FORCEINLINE int32 GetTileIndex() { return TileIndex; }
};
