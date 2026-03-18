#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "FUActivatableWidget.generated.h"

UENUM(BlueprintType)
enum class EFUInputMode : uint8
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
class FORGERYUI_API UFUActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

protected:
	/** The desired input mode to use while this UI is activated. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EFUInputMode InputConfig = EFUInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
