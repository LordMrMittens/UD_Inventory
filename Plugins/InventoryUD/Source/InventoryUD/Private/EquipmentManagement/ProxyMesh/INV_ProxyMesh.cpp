// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentManagement/ProxyMesh/INV_ProxyMesh.h"
#include "GameFramework/Character.h"
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
	DelayedInitializeOwner();
	
}

void AINV_ProxyMesh::DelayedInitializeOwner()
{
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		DelayedInitialization();
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!IsValid(PC))
	{
		DelayedInitialization(); 
		return;
	}
	ACharacter* Character = Cast<ACharacter>(PC->GetPawn());
	if (!IsValid(Character)) {
		DelayedInitialization();
		return;
	}
	USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
	if(!IsValid(CharacterMesh))
	{
		DelayedInitialization();
		return;
	}
	SourceMesh = CharacterMesh;
	Mesh->SetSkeletalMesh(SourceMesh->GetSkeletalMeshAsset());
	Mesh->SetAnimInstanceClass(SourceMesh->GetAnimInstance()->GetClass());
	EquipmentComponent->InitializeOwner(PC);
}

void AINV_ProxyMesh::DelayedInitialization()
{
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &ThisClass::DelayedInitializeOwner);
	GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
}


