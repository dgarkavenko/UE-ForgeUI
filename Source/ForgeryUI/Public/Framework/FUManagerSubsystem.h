#pragma once

#include "CoreMinimal.h"
#include "GameUIManagerSubsystem.h"
#include "FUManagerSubsystem.generated.h"

/**
 * Game UI manager subsystem that syncs HUD visibility with the root layout.
 * Collapses the layout when HUD is hidden, shows as SelfHitTestInvisible when visible.
 */
UCLASS()
class FORGERYUI_API UFUManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()

public:
	UFUManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	bool Tick(float DeltaTime);
	FTSTicker::FDelegateHandle TickHandle;
};
