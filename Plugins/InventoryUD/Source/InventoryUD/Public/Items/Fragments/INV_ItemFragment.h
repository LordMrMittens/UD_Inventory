

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "INV_ItemFragment.generated.h"

class APlayerController;
class UINV_CompositeBase;
class AINV_EquipActor;


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

	virtual void Manifest() {}
	FGameplayTag GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(FGameplayTag Tag) { FragmentTag = Tag; }
};

/*
* Fragment for assimilation into widget
*/
USTRUCT(BlueprintType)
struct FINV_InventoryItemFragment : public FINV_ItemFragment
{
	GENERATED_BODY()
public:
	virtual void Assimilate(UINV_CompositeBase* Composite) const;
protected:
	bool MatchesWidgetTag(const UINV_CompositeBase* Composite) const;
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

//Inventory Item Fragments

USTRUCT(BlueprintType)
struct FINV_ImageFragment : public FINV_InventoryItemFragment
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon{ nullptr };
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions{ 44.f,44.f };
public:
	UTexture2D* GetIcon() const { return Icon; }
	virtual void Assimilate(UINV_CompositeBase* Composite) const override;

};

USTRUCT(BlueprintType)
struct FINV_TextFragment : public FINV_InventoryItemFragment
{
	GENERATED_BODY()
public:
	virtual void Assimilate(UINV_CompositeBase* Composite) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText FragmentText;
public:
	void SetText(const FText& Text) { FragmentText = Text; }
	FText GetText() const { return FragmentText; }
};
USTRUCT(BlueprintType)
struct FINV_LabeledNumberFragment : public FINV_InventoryItemFragment
{
	GENERATED_BODY()
public:
	virtual void Assimilate(UINV_CompositeBase* Composite) const override;

	virtual void Manifest() override;

	//when manifesting for the first time this fragment randomises, it should then retain the same value
	bool bRandomiseOnManifest{ true };

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText Text_Label{};
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float Value{0.f};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float MinValue{0.f};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float MaxValue{10.f};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseLabel{ false };
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseValue{ false };
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MinFractionalDigits{ 1 };
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxFractionalDigits{ 1 };
public:
	float GetValue() const { return Value; }
};


//Consume fragments

USTRUCT(BlueprintType)
struct FINV_ConsumeModifier : public FINV_LabeledNumberFragment
{
	GENERATED_BODY()

public:

	virtual void OnConsume(APlayerController* PlayerController) {}
};


USTRUCT(BlueprintType)
struct FINV_ConsumableFragment : public FINV_InventoryItemFragment
{
	GENERATED_BODY()

public:
	virtual void Assimilate(UINV_CompositeBase* Composite) const override;
	virtual void OnConsume(APlayerController* PlayerController);
	virtual void Manifest() override;

private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FINV_ConsumeModifier>> ConsumeModifiers;

};

USTRUCT(BlueprintType)
struct FINV_HealthPotionFragment : public FINV_ConsumeModifier
{
	GENERATED_BODY()

public:

	virtual void OnConsume(APlayerController* PlayerController) override;


};
USTRUCT(BlueprintType)
struct FINV_ManaPotionFragment : public FINV_ConsumeModifier
{
	GENERATED_BODY()

public:

	virtual void OnConsume(APlayerController* PlayerController) override;

};

//Equipment Fragments

USTRUCT(BlueprintType)
struct FINV_EquipModifier : public FINV_LabeledNumberFragment
{
	GENERATED_BODY()

public:

	virtual void OnEquip(APlayerController* PlayerController) {}
	virtual void OnUnequip(APlayerController* PlayerController) {}
};
USTRUCT(BlueprintType)
struct FINV_StrengthModifier : public FINV_EquipModifier
{
	GENERATED_BODY()

public:

	virtual void OnEquip(APlayerController* PlayerController) override;
	virtual void OnUnequip(APlayerController* PlayerController) override;
};

USTRUCT(BlueprintType)
struct FINV_EquipmentFragment : public FINV_InventoryItemFragment
{
	GENERATED_BODY()

public:
	bool bEquipped{ false };
	virtual void Manifest() override;
	void OnEquip(APlayerController* PlayerController);
	void OnUnequip(APlayerController* PlayerController);
	virtual void Assimilate(UINV_CompositeBase* Composite) const override;

	AINV_EquipActor* SpawnAttachedActor(USkeletalMeshComponent* AttachMesh) const;
	void DestroyAttachedActor();

	FGameplayTag GetEquipmentType() const { return EquipmentType; }

	void SetEquippedActor(AINV_EquipActor* EquipActor);
private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FINV_EquipModifier>> EquipModifiers;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<AINV_EquipActor> EquipActorClass{ nullptr };

	TWeakObjectPtr<AINV_EquipActor> EquippedActor{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FName SocketAttachPoint{NAME_None};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag EquipmentType{FGameplayTag::EmptyTag};

};