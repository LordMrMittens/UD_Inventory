#include "InventoryManagement/FastArray/INV_FastArray.h"
#include "Items/INV_InventoryItem.h"
#include "Items/Components/INV_ItemComponent.h"
#include "InventoryManagement/Components/INV_InventoryComponent.h"

TArray<UINV_InventoryItem*> FINV_InventoryFastArray::GetAllItems() const
{
	TArray<UINV_InventoryItem*> Results;
	Results.Reserve(Entries.Num());
	for (const FINV_InventoryEntry& Entry : Entries) {
		if (!IsValid(Entry.Item)) continue;
		Results.Add(Entry.Item);
	}
	return Results;
}


void FINV_InventoryFastArray::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	UINV_InventoryComponent* IC = Cast<UINV_InventoryComponent>(OwnerComponent);
	if(!IsValid(IC)) return;

	for (int32 Index : RemovedIndices) {
		IC->OnItemRemoved.Broadcast(Entries[Index].Item);
	}
}
void FINV_InventoryFastArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	UINV_InventoryComponent* IC = Cast<UINV_InventoryComponent>(OwnerComponent);
	if (!IsValid(IC)) return;

	for (int32 Index : AddedIndices) {
		IC->OnItemAdded.Broadcast(Entries[Index].Item);
	}
}

UINV_InventoryItem* FINV_InventoryFastArray::AddEntry(UINV_ItemComponent* ItemComponent)
{
	//TODO once item component is more complete
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	UINV_InventoryComponent* IC = Cast<UINV_InventoryComponent>(OwnerComponent);
	if (!IsValid(IC)) return nullptr;

	FINV_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = ItemComponent->GetItemManifest().Manifest(OwningActor);
	IC->AddRepSubObject(NewEntry.Item);
	MarkItemDirty(NewEntry);
	return NewEntry.Item;
}

UINV_InventoryItem* FINV_InventoryFastArray::AddEntry(UINV_InventoryItem* Item)
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	FINV_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = Item;

	MarkItemDirty(NewEntry);

	return Item;
}

void FINV_InventoryFastArray::RemoveEntry(UINV_InventoryItem* Item)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt) {
		FINV_InventoryEntry& Entry = *EntryIt;
		if (Entry.Item == Item)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}

}
