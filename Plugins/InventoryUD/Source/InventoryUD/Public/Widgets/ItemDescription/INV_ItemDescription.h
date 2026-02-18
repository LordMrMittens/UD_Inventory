

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "INV_ItemDescription.generated.h"

class USizeBox;

UCLASS()
class INVENTORYUD_API UINV_ItemDescription : public UUserWidget
{
	GENERATED_BODY()

public:

	FVector2D GetBoxSize() const;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox;
};
