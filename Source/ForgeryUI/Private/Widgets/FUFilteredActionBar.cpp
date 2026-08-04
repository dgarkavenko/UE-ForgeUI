#include "Widgets/FUFilteredActionBar.h"

#include "Input/CommonBoundActionButtonInterface.h"
#include "Input/UIActionBindingHandle.h"

void UFUFilteredActionBar::NativeOnActionButtonCreated(ICommonBoundActionButtonInterface* ActionButton, const FUIActionBindingHandle& RepresentedAction)
{
	// The base bar's filter is private, so filtered-out entries are still created and collapsed here.
	const bool bListed = ActionNames.Contains(RepresentedAction.GetActionName());
	if (bListed == bExcludeListedActions)
	{
		if (UUserWidget* ButtonWidget = Cast<UUserWidget>(ActionButton))
		{
			ButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
