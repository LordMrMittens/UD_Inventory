

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "INV_CompositeBase.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYUD_API UINV_CompositeBase : public UUserWidget
{
	GENERATED_BODY()

public:

	void Expand();

	virtual void Collapse();

	using FuncType = TFunction<void(UINV_CompositeBase*)>;
	virtual void ApplyFunction(FuncType Function) {}

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag FragmentTag;

public:

FGameplayTag GetFragmentTag() const { return FragmentTag; }
void SetFragmentTag(const  FGameplayTag& Tag) { FragmentTag = Tag; }

};
