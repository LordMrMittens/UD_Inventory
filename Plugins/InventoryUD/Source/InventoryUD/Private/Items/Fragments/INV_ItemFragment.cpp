


#include "Items/Fragments/INV_ItemFragment.h"

void FINV_HealthPotionFragment::OnConsume(APlayerController* PlayerController)
{

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Health Potion Consumed!Healed %f HP"), HealthAmount));
}

void FINV_ManaPotionFragment::OnConsume(APlayerController* PlayerController)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Mana Potion Consumed! Replenished %f MP"), ManaAmount));
}
