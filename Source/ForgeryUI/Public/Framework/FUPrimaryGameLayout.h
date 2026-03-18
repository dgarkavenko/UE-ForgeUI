#pragma once

#include "CoreMinimal.h"
#include "PrimaryGameLayout.h"
#include "FUPrimaryGameLayout.generated.h"

/**
 * Primary game layout with 4 UI layer stacks:
 * - Game: in-game HUD widgets
 * - GameMenu: pause/inventory menus
 * - Menu: main menu / frontend
 * - Modal: modal dialogs / popups
 */
UCLASS()
class FORGERYUI_API UFUPrimaryGameLayout : public UPrimaryGameLayout
{
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonActivatableWidgetStack* Game_Stack;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonActivatableWidgetStack* GameMenu_Stack;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonActivatableWidgetStack* Menu_Stack;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonActivatableWidgetStack* Modal_Stack;
};
