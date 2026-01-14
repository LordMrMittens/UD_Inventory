

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "INVPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UINV_HUDWidget;
/**
 * 
 */
UCLASS()
class INVENTORYUD_API AINVPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AINVPlayerController();
	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputMappingContext> DefaultIMC;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputAction> PrimaryInteractAction;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UINV_HUDWidget> HUDWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UINV_HUDWidget> HUDWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	double TraceLength = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TEnumAsByte<ECollisionChannel> ItemTraceChannel;
	
	void PrimaryInteract();
	void CreateHUDWidget();
	void TraceForItem();
	
	TWeakObjectPtr<AActor> CurrentActor;
	TWeakObjectPtr<AActor> LastActor;
};
