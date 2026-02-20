// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Composite/INV_Leaf.h"
#include "INV_Leaf_Text.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class INVENTORYUD_API UINV_Leaf_Text : public UINV_Leaf
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;

	void SetLeafText(const FText& Text) const;

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSize{ 12 };

	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UTextBlock> Text_LeafText;
};
