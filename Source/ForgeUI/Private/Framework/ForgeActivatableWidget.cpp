#include "Framework/ForgeActivatableWidget.h"

TOptional<FUIInputConfig> UForgeActivatableWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case EForgeInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case EForgeInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case EForgeInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	case EForgeInputMode::Default:
	default:
		return Super::GetDesiredInputConfig();
	}
}
