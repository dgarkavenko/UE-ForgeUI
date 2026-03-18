#include "Framework/FUPrimaryGameLayout.h"

#include "Framework/FULayers.h"

void UFUPrimaryGameLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterLayer(FULayers::TAG_UI_LAYER_GAME, Game_Stack);
	RegisterLayer(FULayers::TAG_UI_LAYER_GAMEMENU, GameMenu_Stack);
	RegisterLayer(FULayers::TAG_UI_LAYER_MENU, Menu_Stack);
	RegisterLayer(FULayers::TAG_UI_LAYER_MODAL, Modal_Stack);
}
