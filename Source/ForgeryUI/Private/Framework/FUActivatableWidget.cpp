#include "Framework/FUActivatableWidget.h"

TOptional<FUIInputConfig> UFUActivatableWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case EFUInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case EFUInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case EFUInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	case EFUInputMode::Default:
	default:
		return Super::GetDesiredInputConfig();
	}
}
