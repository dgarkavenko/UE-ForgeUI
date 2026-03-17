#pragma once

#include "CoreMinimal.h"
#include "Widgets/ForgeButtonBase.h"
#include "Widgets/ForgeTabListWidgetBase.h"
#include "ForgeTabButtonBase.generated.h"

class UCommonLazyImage;

/**
 * Tab button with icon support via CommonLazyImage.
 */
UCLASS()
class FORGEUI_API UForgeTabButtonBase : public UForgeButtonBase, public IForgeTabButtonInterface
{
	GENERATED_BODY()

public:
	void SetIconFromLazyObject(const TSoftObjectPtr<UObject>& LazyObject) const;
	void SetIconBrush(const FSlateBrush& Brush) const;

protected:
	UFUNCTION()
	virtual void SetTabLabelInfo_Implementation(const FForgeTabDescriptor& TabLabelInfo) override;

private:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonLazyImage> LazyImage_Icon;
};
