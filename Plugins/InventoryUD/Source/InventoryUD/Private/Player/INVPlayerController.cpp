


#include "Player/INVPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Widgets/HUD/INV_HUDWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Items/Components/INV_ItemComponent.h"
#include "Interaction/INV_Highlightable.h"
#include "InventoryManagement/Components/INV_InventoryComponent.h"


AINVPlayerController::AINVPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	//initialise default values
	TraceLength = 500.f;
	ItemTraceChannel = ECC_GameTraceChannel1;
}

void AINVPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TraceForItem();
}

void AINVPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem <UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (IsValid(Subsystem)) {
		Subsystem->AddMappingContext(DefaultIMC,0);
	}
	InventoryComponent = FindComponentByClass<UINV_InventoryComponent>();
	CreateHUDWidget();
}

void AINVPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &AINVPlayerController::PrimaryInteract);
	EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &AINVPlayerController::ToggleInventoryMenu);
}

void AINVPlayerController::PrimaryInteract()
{
	if (!CurrentActor.IsValid()) return;
	UINV_ItemComponent* ItemComponent = CurrentActor->FindComponentByClass<UINV_ItemComponent>();
	if (!IsValid(ItemComponent) || !InventoryComponent.IsValid()) return;
	InventoryComponent->TryAddItem(ItemComponent);
	

}

void AINVPlayerController::CreateHUDWidget()
{
	if(!IsLocalPlayerController()) return;
	HUDWidget = CreateWidget<UINV_HUDWidget>(this, HUDWidgetClass);
	if (IsValid(HUDWidget)) {
		HUDWidget->AddToViewport();
	}
}

void AINVPlayerController::TraceForItem()
{
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport)) return;
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FVector2D ViewportCentre = ViewportSize / 2.f;
	FVector TraceStart;
	FVector Forward;
	if (!UGameplayStatics::DeprojectScreenToWorld(this, ViewportCentre, TraceStart, Forward)) return;
	const FVector TraceEnd = TraceStart + Forward * TraceLength;
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ItemTraceChannel);

	LastActor = CurrentActor;
	CurrentActor = HitResult.GetActor();

	if (!CurrentActor.IsValid()) {
		if (IsValid(HUDWidget)) HUDWidget->HidePickupMessage();
	}

	if (CurrentActor == LastActor) return;

	if (CurrentActor.IsValid()) {
		if (UActorComponent* Highlightable = CurrentActor->FindComponentByInterface(UINV_Highlightable::StaticClass()); IsValid(Highlightable)) {
			IINV_Highlightable::Execute_Highlight(Highlightable);
		}

		UINV_ItemComponent* ItemComponent = CurrentActor->FindComponentByClass<UINV_ItemComponent>();

		if (!IsValid(ItemComponent)) return;

		if (IsValid(HUDWidget)) HUDWidget->ShowPickupMessage(ItemComponent->GetPickupMessage());

	}

	if (LastActor.IsValid()) {
		if (UActorComponent* Highlightable = LastActor->FindComponentByInterface(UINV_Highlightable::StaticClass()); IsValid(Highlightable)) {
			IINV_Highlightable::Execute_UnHighlight(Highlightable);
		}
	}
}

void AINVPlayerController::ToggleInventoryMenu()
{
	if (!InventoryComponent.IsValid()) return;
	InventoryComponent->ToggleInventoryMenu();
	if (InventoryComponent->IsMenuOpen()) {
		HUDWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else {
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}
