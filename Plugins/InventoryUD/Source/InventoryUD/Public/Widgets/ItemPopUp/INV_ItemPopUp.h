#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "INV_ItemPopUp.generated.h"

class UButton;
class USlider;
class UTextBlock;
class USizeBox;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FPopUpMenuSplit, int32, SplitAmount, int32, Index);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuDrop, int32, Index);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuConsume, int32, Index);
/**
 *
 */
UCLASS()
class INVENTORYUD_API UINV_ItemPopUp : public UUserWidget
{
	GENERATED_BODY()
public:

	FPopUpMenuSplit PopUpSplit;
	FPopUpMenuDrop PopUpDrop;
	FPopUpMenuConsume PopUpConsume;

	virtual void NativeOnInitialized() override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	int32 GetSplitAmount() const;

	FVector2D GetBoxSize() const;

	void SetSliderParams(const float Max, const float Value) const;

	void CollapseSplitButton() const;
	void CollapseConsumeButton() const;

private:
	UPROPERTY(EditAnywhere, Meta =(BindWidget))
	TObjectPtr<UButton> Button_Split;

	UPROPERTY(EditAnywhere, Meta = (BindWidget))
	TObjectPtr<UButton> Button_Drop;

	UPROPERTY(EditAnywhere, Meta = (BindWidget))
	TObjectPtr<UButton> Button_Consume;

	UPROPERTY(EditAnywhere, Meta = (BindWidget))
	TObjectPtr<USlider> Slider_Split;

	UPROPERTY(EditAnywhere, Meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SplitAmount;

	UPROPERTY(EditAnywhere, Meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;

	int32 GridIndex{ INDEX_NONE };

	UFUNCTION()
	void SplitButtonClicked();
	UFUNCTION()
	void DropButtonClicked();
	UFUNCTION()
	void ConsumeButtonClicked();
	UFUNCTION()
	void SliderValueChanged(float Value);



public:

	void SetGridIndex(const int32 Index) { GridIndex = Index; }
	int32 GetGridIndex() const { return GridIndex; }
};
