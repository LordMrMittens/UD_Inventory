


#include "Player/INVPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Widgets/HUD/INV_HUDWidget.h"

void AINVPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem <UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (IsValid(Subsystem)) {
		Subsystem->AddMappingContext(DefaultIMC,0);
	}
	CreateHUDWidget();
}

void AINVPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &AINVPlayerController::PrimaryInteract);
}

void AINVPlayerController::PrimaryInteract()
{
	UE_LOG(LogTemp, Error, TEXT("Primary Interact Action"));
}

void AINVPlayerController::CreateHUDWidget()
{
	if(!IsLocalPlayerController()) return;
	HUDWidget = CreateWidget<UINV_HUDWidget>(this, HUDWidgetClass);
	if (IsValid(HUDWidget)) {
		HUDWidget->AddToViewport();
	}
}
