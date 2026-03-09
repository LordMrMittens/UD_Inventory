


#include "Items/Fragments/INV_ItemFragment.h"
#include "EquipmentManagement/INV_EquipActor.h"
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

void FINV_StrengthModifier::OnEquip(APlayerController* PlayerController)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Item Equipped Gained %f Strength"), GetValue()));
}

void FINV_StrengthModifier::OnUnequip(APlayerController* PlayerController)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Item Unequipped Lost %f Strength"), GetValue()));
}

void FINV_EquipmentFragment::Manifest()
{
	FINV_InventoryItemFragment::Manifest();
	for (auto& Modifier : EquipModifiers)
	{
		auto& ModifierReference = Modifier.GetMutable();
		ModifierReference.Manifest();
	}
}

void FINV_EquipmentFragment::OnEquip(APlayerController* PlayerController)
{
	if (bEquipped) return;
	bEquipped = true;
	for (auto& Modifier : EquipModifiers) {
		auto& ModifierReference = Modifier.GetMutable();
		ModifierReference.OnEquip(PlayerController);
	}
}

void FINV_EquipmentFragment::OnUnequip(APlayerController* PlayerController) 
{
	if (!bEquipped) return;
	bEquipped = false;
	for (auto& Modifier : EquipModifiers) {
		auto& ModifierReference = Modifier.GetMutable();
		ModifierReference.OnUnequip(PlayerController);
	}
}

void FINV_EquipmentFragment::Assimilate(UINV_CompositeBase* Composite) const
{
	FINV_InventoryItemFragment::Assimilate(Composite);
	for (const auto& Modifier : EquipModifiers) {
		const auto& ModifierReference = Modifier.Get();
		ModifierReference.Assimilate(Composite);
	}
}

AINV_EquipActor* FINV_EquipmentFragment::SpawnAttachedActor(USkeletalMeshComponent* AttachMesh) const
{
	if (!IsValid(EquipActorClass)|| !IsValid(AttachMesh)) return nullptr;

	AINV_EquipActor* SpawnedActor = AttachMesh->GetWorld()->SpawnActor<AINV_EquipActor>(EquipActorClass);
	SpawnedActor->AttachToComponent(AttachMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketAttachPoint);
	return SpawnedActor;
}

void FINV_EquipmentFragment::DestroyAttachedActor()
{
	if (EquippedActor.IsValid())
	{
		EquippedActor->Destroy();
	}
}
