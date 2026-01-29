

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "INV_ItemFragment.generated.h"


USTRUCT(BlueprintType)
struct FINV_ItemFragment
{
	GENERATED_BODY()

	FINV_ItemFragment() {}
	FINV_ItemFragment(const FINV_ItemFragment&) = default;
	FINV_ItemFragment& operator = (const FINV_ItemFragment&) = default;
	FINV_ItemFragment(FINV_ItemFragment&&) = default;
	FINV_ItemFragment& operator = (FINV_ItemFragment&&) = default;
	virtual ~FINV_ItemFragment() {}

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag FragmentTag = FGameplayTag::EmptyTag;

public:
	FGameplayTag GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(FGameplayTag Tag) { FragmentTag = Tag; }
};
USTRUCT(BlueprintType)
struct FINV_GridFragment : public FINV_ItemFragment
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint GridSize{ 1,1 };
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float GridPadding{ 0.f };
public:
	FIntPoint GetGridSize() { return GridSize; }
	void SetGridSize(FIntPoint Size) { GridSize = Size; }
	float GetGridPadding() { return GridPadding; }
	void SetGridPadding(float Padding) { GridPadding = Padding; }

};