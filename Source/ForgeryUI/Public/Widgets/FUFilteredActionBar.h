#pragma once

#include "CoreMinimal.h"
#include "Input/CommonBoundActionBar.h"
#include "FUFilteredActionBar.generated.h"

/**
 * Bound action bar that only displays a declared subset of actions, so multiple bars can split the action set.
 */
UCLASS(ClassGroup = UI, meta = (Category = "Forgery UI"))
class FORGERYUI_API UFUFilteredActionBar : public UCommonBoundActionBar
{
	GENERATED_BODY()

protected:
	virtual void NativeOnActionButtonCreated(ICommonBoundActionButtonInterface* ActionButton, const FUIActionBindingHandle& RepresentedAction) override;

private:
	// Action names are the CommonUI input action data table row names (e.g. "Back", "NavLeft").
	UPROPERTY(EditAnywhere, Category = Filter)
	TArray<FName> ActionNames;

	UPROPERTY(EditAnywhere, Category = Filter)
	bool bExcludeListedActions = false;
};
