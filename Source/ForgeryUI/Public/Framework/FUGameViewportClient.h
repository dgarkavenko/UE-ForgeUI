#pragma once

#include "CoreMinimal.h"
#include "CommonGameViewportClient.h"
#include "FUGameViewportClient.generated.h"

/**
 * Game viewport client extending CommonGameViewportClient.
 * Projects can subclass this for custom viewport behavior.
 */
UCLASS()
class FORGERYUI_API UFUGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()
};
