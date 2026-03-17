#pragma once

#include "CoreMinimal.h"
#include "CommonGameViewportClient.h"
#include "ForgeGameViewportClient.generated.h"

/**
 * Game viewport client extending CommonGameViewportClient.
 * Projects can subclass this for custom viewport behavior.
 */
UCLASS()
class FORGEUI_API UForgeGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()
};
