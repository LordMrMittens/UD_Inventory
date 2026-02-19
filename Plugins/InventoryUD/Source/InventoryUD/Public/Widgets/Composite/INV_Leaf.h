

#pragma once

#include "CoreMinimal.h"
#include "Widgets\Composite\INV_CompositeBase.h"
#include "INV_Leaf.generated.h"


UCLASS( )
class INVENTORYUD_API UINV_Leaf : public UINV_CompositeBase
{
	GENERATED_BODY()
public:
	virtual void ApplyFunction(FuncType Function) override;
	
};
