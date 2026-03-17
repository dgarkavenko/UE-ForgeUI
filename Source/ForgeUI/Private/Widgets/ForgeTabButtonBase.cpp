#include "Widgets/ForgeTabButtonBase.h"

#include "CommonLazyImage.h"

void UForgeTabButtonBase::SetIconFromLazyObject(const TSoftObjectPtr<UObject>& LazyObject) const
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrushFromLazyDisplayAsset(LazyObject);
	}
}

void UForgeTabButtonBase::SetIconBrush(const FSlateBrush& Brush) const
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrush(Brush);
	}
}

void UForgeTabButtonBase::SetTabLabelInfo_Implementation(const FForgeTabDescriptor& TabLabelInfo)
{
	SetButtonText(TabLabelInfo.TabText);
	SetIconBrush(TabLabelInfo.IconBrush);
}
