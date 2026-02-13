


#include "Widgets\ItemPopUp\INV_ItemPopUp.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/SizeBox.h"

void UINV_ItemPopUp::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Split->OnClicked.AddDynamic(this, &ThisClass::SplitButtonClicked);
	Button_Drop->OnClicked.AddDynamic(this, &ThisClass::DropButtonClicked);
	Button_Consume->OnClicked.AddDynamic(this, &ThisClass::ConsumeButtonClicked);
	Slider_Split->OnValueChanged.AddDynamic(this, &ThisClass::SliderValueChanged);
}

void UINV_ItemPopUp::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	RemoveFromParent();
}

void UINV_ItemPopUp::SplitButtonClicked()
{
	if (PopUpSplit.ExecuteIfBound(GetSplitAmount(), GridIndex)) 
	{
		RemoveFromParent();
	}
}

void UINV_ItemPopUp::DropButtonClicked()
{
	if (PopUpDrop.ExecuteIfBound(GridIndex))
	{
		RemoveFromParent();
	}
}

void UINV_ItemPopUp::ConsumeButtonClicked()
{
	if (PopUpConsume.ExecuteIfBound(GridIndex))
	{
		RemoveFromParent();
	}
}

void UINV_ItemPopUp::SliderValueChanged(float Value)
{
	Text_SplitAmount->SetText(FText::AsNumber(FMath::FloorToInt(Value)));
}

void UINV_ItemPopUp::SetSliderParams(const float Max, const float Value) const
{
	Slider_Split->SetMaxValue(Max);
	Slider_Split->SetMinValue(1);
	Slider_Split->SetValue(Value);
	Text_SplitAmount->SetText(FText::AsNumber(FMath::FloorToInt(Value)));
}

void UINV_ItemPopUp::CollapseSplitButton() const
{
	Button_Split->SetVisibility(ESlateVisibility::Collapsed);
	Slider_Split->SetVisibility(ESlateVisibility::Collapsed);
	Text_SplitAmount->SetVisibility(ESlateVisibility::Collapsed);
}

void UINV_ItemPopUp::CollapseConsumeButton() const
{
	Button_Consume->SetVisibility(ESlateVisibility::Collapsed);
}

int32 UINV_ItemPopUp::GetSplitAmount() const {
	return FMath::FloorToInt32(Slider_Split->GetValue());
}

FVector2D UINV_ItemPopUp::GetBoxSize() const {
	return FVector2D(SizeBox_Root->GetWidthOverride(), SizeBox_Root->GetHeightOverride());
}


