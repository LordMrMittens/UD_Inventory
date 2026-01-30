

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Manifest/INV_ItemManifest.h"
#include "INV_InventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYUD_API UINV_InventoryItem : public UObject
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }

	void SetItemManifest(const FINV_ItemManifest& Manifest);

	const FINV_ItemManifest& GetItemManifest() const { return ItemManifest.Get<FINV_ItemManifest>(); }
	FINV_ItemManifest& GetItemManifestMutable() { return ItemManifest.GetMutable<FINV_ItemManifest>(); }
private:

	UPROPERTY(VisibleAnywhere, meta = (BaseStruct = "/Script/InventoryUD.INV_ItemManifest"), Replicated ,Category = "Inventory")
	FInstancedStruct ItemManifest;


};

template <typename FragmentType>
const FragmentType* GetFragment(const UINV_InventoryItem* Item, const FGameplayTag& Tag)
{
	if (!IsValid(Item))  return nullptr;

	const FINV_ItemManifest& Manifest = Item->GetItemManifest();
	return Manifest.GetFragmentOfTypeWithTag<FragmentType>(Tag);
};