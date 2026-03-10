// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "INV_ProxyMesh.generated.h"

class UINV_EquipmentComponent;

UCLASS()
class INVENTORYUD_API AINV_ProxyMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AINV_ProxyMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:

	//Mesh on actual player character
	TWeakObjectPtr<USkeletalMeshComponent> SourceMesh;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UINV_EquipmentComponent> EquipmentComponent;

	//proxy mesh on inventory menu
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<USkeletalMeshComponent> Mesh;

	FTimerHandle TimerForNextTick;
	void DelayedInitializeOwner();
	void DelayedInitialization();
};
