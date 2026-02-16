// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Components/INV_ItemComponent.h"
#include "Net/UnrealNetwork.h"

UINV_ItemComponent::UINV_ItemComponent()
{

	PrimaryComponentTick.bCanEverTick = false;
	//initialise default values
	PickupMessage = "E - Pickup";
}

void UINV_ItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, ItemManifest);
}

void UINV_ItemComponent::PickedUp()
{
	OnPickedUp();
	GetOwner()->Destroy();
}

void UINV_ItemComponent::InitItemManifest(FINV_ItemManifest CopyOfManifest)
{
	ItemManifest = CopyOfManifest;
}

