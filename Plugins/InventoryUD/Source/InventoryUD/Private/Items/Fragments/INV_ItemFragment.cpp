


#include "Items/Fragments/INV_ItemFragment.h"
#include "Widgets/Composite/INV_CompositeBase.h"

void FINV_InventoryItemFragment::Assimilate(UINV_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
	Composite->Expand();
}

bool FINV_InventoryItemFragment::MatchesWidgetTag(const UINV_CompositeBase* Composite) const
{

	return Composite->GetFragmentTag().MatchesTagExact(Composite->GetFragmentTag());
}

void FINV_HealthPotionFragment::OnConsume(APlayerController* PlayerController)
{

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Health Potion Consumed!Healed %f HP"), HealthAmount));
}

void FINV_ManaPotionFragment::OnConsume(APlayerController* PlayerController)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Mana Potion Consumed! Replenished %f MP"), ManaAmount));
}

