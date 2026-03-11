// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CharacterDisplay/INV_CharacterDisplay.h"
#include "EquipmentManagement/ProxyMesh/INV_ProxyMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UINV_CharacterDisplay::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AINV_ProxyMesh::StaticClass(), Actors);
	if (!Actors.IsValidIndex(0)) return;
	AINV_ProxyMesh* ProxyMesh = Cast<AINV_ProxyMesh>(Actors[0]);
	if (!IsValid(ProxyMesh)) return;
	Mesh = ProxyMesh->GetProxyMesh();
}

void UINV_CharacterDisplay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!bIsDragging) return;
	LastPosition = CurrentPosition;
	CurrentPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);

	const float HorizontalDelta = LastPosition.X - CurrentPosition.X;

	if (!Mesh.IsValid()) return
		Mesh->AddRelativeRotation(FRotator(0, HorizontalDelta, 0));
}

FReply UINV_CharacterDisplay::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	CurrentPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	LastPosition = CurrentPosition;
	bIsDragging = true;
	return FReply::Handled();
}

FReply UINV_CharacterDisplay::NativeOnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& InMouseEvent)
{
	bIsDragging = false;
	return Super::NativeOnMouseButtonUp(MyGeometry, InMouseEvent);
}

void UINV_CharacterDisplay::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	bIsDragging = false;
}
