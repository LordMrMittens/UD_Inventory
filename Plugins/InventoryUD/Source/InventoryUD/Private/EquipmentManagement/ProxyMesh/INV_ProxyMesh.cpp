// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentManagement/ProxyMesh/INV_ProxyMesh.h"
#include "EquipmentManagement/Components/INV_EquipmentComponent.h"

AINV_ProxyMesh::AINV_ProxyMesh()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	EquipmentComponent = CreateDefaultSubobject<UINV_EquipmentComponent>(TEXT("EquipmentComponent"));
	EquipmentComponent->SetOwningSkeletalMesh(Mesh);
	EquipmentComponent->SetIsProxy(true);
}

// Called when the game starts or when spawned
void AINV_ProxyMesh::BeginPlay()
{
	Super::BeginPlay();
	
}


