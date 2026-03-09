// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "INV_EquipActor.generated.h"

UCLASS()
class INVENTORYUD_API AINV_EquipActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AINV_EquipActor();



private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag EquipmentType;

public:

FGameplayTag GetEquipmentType() const { return EquipmentType; }
void SetEquipmentType(const  FGameplayTag Type) { EquipmentType = Type; }

};
