
#include "Items/Manifest/INV_ItemManifest.h"
#include "Items/Components/INV_ItemComponent.h"
#include "Items/INV_InventoryItem.h"
#include "Items/Fragments/INV_ItemFragment.h"
#include "Widgets/Composite/INV_CompositeBase.h"


void FINV_ItemManifest::SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (!IsValid(PickupActorClass) || !IsValid(WorldContextObject)) return;
	AActor* SpawnedActor = WorldContextObject->GetWorld()->SpawnActor<AActor>(PickupActorClass, SpawnLocation, SpawnRotation);
	if (!IsValid(SpawnedActor)) return;
	UINV_ItemComponent* ItemComp = SpawnedActor->GetComponentByClass<UINV_ItemComponent>();
	check(ItemComp);

	ItemComp->InitItemManifest(*this);
}

UINV_InventoryItem* FINV_ItemManifest::Manifest(UObject* NewOuter)
{
	UINV_InventoryItem* Item = NewObject<UINV_InventoryItem>(NewOuter, UINV_InventoryItem::StaticClass());
	Item->SetItemManifest(*this);
	for (auto& Fragment : Item->GetItemManifestMutable().GetFragmentsMutable()) {
		Fragment.GetMutable().Manifest();
	}
	ClearFragments();
	return Item;
}

void FINV_ItemManifest::AssimilateInventoryFragments(UINV_CompositeBase* Composite) const {

	const auto& InventoryItemFragments = GetAllFragmentsOfType<FINV_InventoryItemFragment>();
	for (const auto& Fragment : InventoryItemFragments)
	{
		Composite->ApplyFunction([Fragment](UINV_CompositeBase* Widget) {
			Fragment->Assimilate(Widget);
			});

	}
}

void  FINV_ItemManifest::ClearFragments() {
	for (auto& Fragment : ItemFragments) {
		Fragment.Reset();
	}
	ItemFragments.Empty();
}
