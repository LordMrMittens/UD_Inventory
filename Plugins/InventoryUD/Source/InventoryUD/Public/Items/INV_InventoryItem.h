

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

	void SetItemManifest(const FINV_ItemManifest& Manifest);
private:

	UPROPERTY(VisibleAnywhere, meta = (BaseStruct = "/Script/InventoryUD.INV_ItemManifest"), Replicated ,Category = "Inventory")
	FInstancedStruct ItemManifest;
};
