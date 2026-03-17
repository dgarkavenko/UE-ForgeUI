#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "ForgeActivatableWidget.generated.h"

UENUM(BlueprintType)
enum class EForgeInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

/**
 * Base activatable widget with simplified input mode configuration.
 */
UCLASS()
class FORGEUI_API UForgeActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

protected:
	/** The desired input mode to use while this UI is activated. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EForgeInputMode InputConfig = EForgeInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
