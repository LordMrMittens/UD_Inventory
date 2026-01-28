// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Manifest/INV_ItemManifest.h"
#include "INV_ItemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class INVENTORYUD_API UINV_ItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UINV_ItemComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	
private:	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FString PickupMessage;

	UPROPERTY(Replicated, EditAnywhere, Category = "Inventory")
	FINV_ItemManifest ItemManifest;

public:
	FORCEINLINE FString GetPickupMessage() const { return PickupMessage; }
	FORCEINLINE FINV_ItemManifest GetItemManifest() const { return ItemManifest; }
};
