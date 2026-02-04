
#include "Items/INV_InventoryItem.h"
#include "Items/Fragments/INV_ItemFragment.h"
#include "Net/UnrealNetwork.h"

void UINV_InventoryItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, ItemManifest)
	DOREPLIFETIME(ThisClass, TotalStackCount)
}

void UINV_InventoryItem::SetItemManifest(const FINV_ItemManifest& Manifest)
{
	ItemManifest = FInstancedStruct::Make<FINV_ItemManifest>(Manifest);
}

bool UINV_InventoryItem::IsStackable() const
{
	const FINV_StackableFragment* Stackable = GetItemManifest().GetFragmentOfType<FINV_StackableFragment>();
	return Stackable != nullptr;
}
