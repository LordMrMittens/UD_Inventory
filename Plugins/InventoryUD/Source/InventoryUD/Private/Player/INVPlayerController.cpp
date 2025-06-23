


#include "Player/INVPlayerController.h"
#include "InventoryUD.h"

void AINVPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogInventory, Log, TEXT("Began Play"));
}
