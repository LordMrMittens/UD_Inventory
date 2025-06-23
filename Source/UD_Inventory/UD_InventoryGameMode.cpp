// Copyright Epic Games, Inc. All Rights Reserved.

#include "UD_InventoryGameMode.h"
#include "UD_InventoryCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUD_InventoryGameMode::AUD_InventoryGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
