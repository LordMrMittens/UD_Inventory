

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "INV_ItemFragment.generated.h"

class APlayerController;


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
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories="Fragments"))
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

	FIntPoint GetGridSize() const { return GridSize; }
	void SetGridSize(const FIntPoint Size) { GridSize = Size; }
	float GetGridPadding() const { return GridPadding; }
	void SetGridPadding(const float Padding) { GridPadding = Padding; }

};

USTRUCT(BlueprintType)
struct FINV_ImageFragment : public FINV_ItemFragment
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon{ nullptr };
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions{ 44.f,44.f };
public:
	UTexture2D* GetIcon() const { return Icon; }

};

USTRUCT(BlueprintType)
struct FINV_StackableFragment : public FINV_ItemFragment
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxStackSize{ 10 };
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 StackCount{ 1 };

public:
	
	int32 GetMaxStackSize() const { return MaxStackSize; }
	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(int32 Count) { StackCount = Count; }

};

USTRUCT(BlueprintType)
struct FINV_ConsumableFragment : public FINV_ItemFragment
{
	GENERATED_BODY()

public:

	virtual void OnConsume(APlayerController* PlayerController) {}


};

USTRUCT(BlueprintType)
struct FINV_HealthPotionFragment : public FINV_ConsumableFragment
{
	GENERATED_BODY()

public:

	virtual void OnConsume(APlayerController* PlayerController) override;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float HealthAmount{ 20.0f };

};
USTRUCT(BlueprintType)
struct FINV_ManaPotionFragment : public FINV_ConsumableFragment
{
	GENERATED_BODY()

public:

	virtual void OnConsume(APlayerController* PlayerController) override;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float ManaAmount{ 20.0f };

};