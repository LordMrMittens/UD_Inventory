#include "InventoryManagement/FastArray/INV_FastArray.h"

TArray<UINV_InventoryItem*> FINV_InventoryFastArray::GetAllItems() const
{
	return TArray<UINV_InventoryItem*>();
}


void FINV_InventoryFastArray::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{

}
void FINV_InventoryFastArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{

}

UINV_InventoryItem* FINV_InventoryFastArray::AddEntry(UINV_InventoryComponent* ItemComponent)
{
	return nullptr;
}

UINV_InventoryItem* FINV_InventoryFastArray::AddEntry(UINV_InventoryItem* Item)
{
	return nullptr;
}

void FINV_InventoryFastArray::RemoveEntry(UINV_InventoryItem* Item)
{
}
