


#include "Items/Fragments/INV_ItemFragment.h"
#include "Widgets/Composite/INV_Leaf_Image.h"
#include "Widgets/Composite/INV_Leaf_Text.h"
#include "Widgets/Composite/INV_Leaf_LabeledValue.h"
#include "Widgets/Composite/INV_CompositeBase.h"

void FINV_InventoryItemFragment::Assimilate(UINV_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
	Composite->Expand();
}

bool FINV_InventoryItemFragment::MatchesWidgetTag(const UINV_CompositeBase* Composite) const
{

	return GetFragmentTag().MatchesTagExact(Composite->GetFragmentTag());
}


void FINV_HealthPotionFragment::OnConsume(APlayerController* PlayerController)
{

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Health Potion Consumed!Healed %f HP"), GetValue()));
}

void FINV_ManaPotionFragment::OnConsume(APlayerController* PlayerController)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Mana Potion Consumed! Replenished %f MP"), GetValue()));
}

void FINV_ImageFragment::Assimilate(UINV_CompositeBase* Composite) const
{
	FINV_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;
	UINV_Leaf_Image* Image = Cast<UINV_Leaf_Image>(Composite);
	if (!IsValid(Image)) return;
	Image->SetImage(Icon);
	Image->SetBoxSize(IconDimensions);
	Image->SetImageSize(IconDimensions);
}

void FINV_TextFragment::Assimilate(UINV_CompositeBase* Composite) const
{
	FINV_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;
	UINV_Leaf_Text* LeafText = Cast<UINV_Leaf_Text>(Composite);
	if (!IsValid(LeafText)) return;

	LeafText->SetLeafText(FragmentText);
}

void FINV_LabeledNumberFragment::Assimilate(UINV_CompositeBase* Composite) const
{
	FINV_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;
	UINV_Leaf_LabeledValue* LeafTextLabeledValue = Cast<UINV_Leaf_LabeledValue>(Composite);
	if (!IsValid(LeafTextLabeledValue)) return;
	LeafTextLabeledValue->SetText_Label(Text_Label, bCollapseLabel);
	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = MinFractionalDigits;
	Options.MaximumFractionalDigits = MaxFractionalDigits;
	LeafTextLabeledValue->SetText_Value(FText::AsNumber(Value, &Options), bCollapseValue);
}
void FINV_ConsumableFragment::Assimilate(UINV_CompositeBase* Composite) const
{
	FINV_InventoryItemFragment::Assimilate(Composite);
	for (const auto& Modifier : ConsumeModifiers) 
	{
		const auto& ModifierReference = Modifier.Get();
		ModifierReference.Assimilate(Composite);
	}
}
void FINV_ConsumableFragment::OnConsume(APlayerController* PlayerController)
{
	for (auto& Modifier : ConsumeModifiers)
	{
		auto& ModifierReference = Modifier.GetMutable();
		ModifierReference.OnConsume(PlayerController);
	}
}

void FINV_ConsumableFragment::Manifest()
{
	for (auto& Modifier : ConsumeModifiers)
	{
		auto& ModifierReference = Modifier.GetMutable();
		ModifierReference.Manifest();
	}
}

void FINV_LabeledNumberFragment::Manifest()
{
	FINV_InventoryItemFragment::Manifest();
	if (bRandomiseOnManifest) {
		Value = FMath::RandRange(MinValue, MaxValue);
	}
		bRandomiseOnManifest = false;
}

