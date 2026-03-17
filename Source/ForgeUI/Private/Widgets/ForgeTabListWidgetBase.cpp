#include "Widgets/ForgeTabListWidgetBase.h"

#include "CommonAnimatedSwitcher.h"
#include "CommonButtonBase.h"

void UForgeTabListWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	SetupTabs();
}

void UForgeTabListWidgetBase::NativeDestruct()
{
	for (FForgeTabDescriptor& TabInfo : PreregisteredTabInfoArray)
	{
		if (TabInfo.CreatedTabContentWidget)
		{
			TabInfo.CreatedTabContentWidget->RemoveFromParent();
			TabInfo.CreatedTabContentWidget = nullptr;
		}
	}

	Super::NativeDestruct();
}

bool UForgeTabListWidgetBase::GetPreregisteredTabInfo(const FName TabNameId, FForgeTabDescriptor& OutTabInfo)
{
	const FForgeTabDescriptor* const FoundTabInfo = PreregisteredTabInfoArray.FindByPredicate([&](FForgeTabDescriptor& TabInfo) -> bool
	{
		return TabInfo.TabId == TabNameId;
	});

	if (!FoundTabInfo)
	{
		return false;
	}

	OutTabInfo = *FoundTabInfo;
	return true;
}

void UForgeTabListWidgetBase::SetTabHiddenState(FName TabNameId, bool bHidden)
{
	for (FForgeTabDescriptor& TabInfo : PreregisteredTabInfoArray)
	{
		if (TabInfo.TabId == TabNameId)
		{
			TabInfo.bHidden = bHidden;
			return;
		}
	}
}

bool UForgeTabListWidgetBase::RegisterDynamicTab(const FForgeTabDescriptor& TabDescriptor)
{
	if (TabDescriptor.bHidden)
	{
		return true;
	}

	PendingTabLabelInfoMap.Add(TabDescriptor.TabId, TabDescriptor);
	return RegisterTab(TabDescriptor.TabId, TabDescriptor.TabButtonType, TabDescriptor.CreatedTabContentWidget);
}

bool UForgeTabListWidgetBase::IsFirstTabActive() const
{
	if (PreregisteredTabInfoArray.IsEmpty())
	{
		return false;
	}

	return GetActiveTab() == PreregisteredTabInfoArray[0].TabId;
}

bool UForgeTabListWidgetBase::IsLastTabActive() const
{
	if (PreregisteredTabInfoArray.IsEmpty())
	{
		return false;
	}

	return GetActiveTab() == PreregisteredTabInfoArray.Last().TabId;
}

bool UForgeTabListWidgetBase::IsTabVisible(FName TabId) const
{
	if (const UCommonButtonBase* Button = GetTabButtonBaseByID(TabId))
	{
		const ESlateVisibility TabVisibility = Button->GetVisibility();
		return TabVisibility == ESlateVisibility::Visible
			|| TabVisibility == ESlateVisibility::HitTestInvisible
			|| TabVisibility == ESlateVisibility::SelfHitTestInvisible;
	}

	return false;
}

int32 UForgeTabListWidgetBase::GetVisibleTabCount()
{
	int32 Result = 0;
	const int32 TabCount = GetTabCount();
	for (int32 Index = 0; Index < TabCount; Index++)
	{
		if (IsTabVisible(GetTabIdAtIndex(Index)))
		{
			Result++;
		}
	}

	return Result;
}

void UForgeTabListWidgetBase::HandlePreLinkedSwitcherChanged()
{
	Super::HandlePreLinkedSwitcherChanged();
}

void UForgeTabListWidgetBase::HandlePostLinkedSwitcherChanged()
{
	Super::HandlePostLinkedSwitcherChanged();
}

void UForgeTabListWidgetBase::HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton)
{
	Super::HandleTabCreation_Implementation(TabId, TabButton);
}

void UForgeTabListWidgetBase::SetupTabs()
{
	for (FForgeTabDescriptor& TabInfo : PreregisteredTabInfoArray)
	{
		if (TabInfo.bHidden)
		{
			continue;
		}

		if (!TabInfo.CreatedTabContentWidget && TabInfo.TabContentType)
		{
			TabInfo.CreatedTabContentWidget = CreateWidget<UCommonUserWidget>(GetOwningPlayer(), TabInfo.TabContentType);
			OnTabContentCreatedNative.Broadcast(TabInfo.TabId, Cast<UCommonUserWidget>(TabInfo.CreatedTabContentWidget));
			OnTabContentCreated.Broadcast(TabInfo.TabId, Cast<UCommonUserWidget>(TabInfo.CreatedTabContentWidget));
		}

		if (UCommonAnimatedSwitcher* CurrentLinkedSwitcher = GetLinkedSwitcher())
		{
			if (!CurrentLinkedSwitcher->HasChild(TabInfo.CreatedTabContentWidget))
			{
				CurrentLinkedSwitcher->AddChild(TabInfo.CreatedTabContentWidget);
			}
		}

		if (GetTabButtonBaseByID(TabInfo.TabId) == nullptr)
		{
			RegisterTab(TabInfo.TabId, TabInfo.TabButtonType, TabInfo.CreatedTabContentWidget);
		}
	}
}
