

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "INVPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYUD_API AINVPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
	
};
