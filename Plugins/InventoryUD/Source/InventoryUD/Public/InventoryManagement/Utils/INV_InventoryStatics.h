// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "INV_InventoryStatics.generated.h"

class UINV_InventoryComponent;
class UINV_ItemComponent;
/**
 * 
 */
UCLASS()
class INVENTORYUD_API UINV_InventoryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UINV_InventoryComponent* GetInventoryComponent(const APlayerController* PlayerController);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static EINV_ItemCategory GetCategoryFromItemComponent(const UINV_ItemComponent* ItemComponent);
	
};
