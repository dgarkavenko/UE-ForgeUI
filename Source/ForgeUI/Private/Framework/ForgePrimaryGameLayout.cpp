#include "Framework/ForgePrimaryGameLayout.h"

#include "Framework/ForgeLayers.h"

void UForgePrimaryGameLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterLayer(ForgeLayers::TAG_UI_LAYER_GAME, Game_Stack);
	RegisterLayer(ForgeLayers::TAG_UI_LAYER_GAMEMENU, GameMenu_Stack);
	RegisterLayer(ForgeLayers::TAG_UI_LAYER_MENU, Menu_Stack);
	RegisterLayer(ForgeLayers::TAG_UI_LAYER_MODAL, Modal_Stack);
}
