#pragma once

#include "CoreMinimal.h"
#include "GameUIManagerSubsystem.h"
#include "ForgeManagerSubsystem.generated.h"

/**
 * Game UI manager subsystem that syncs HUD visibility with the root layout.
 * Collapses the layout when HUD is hidden, shows as SelfHitTestInvisible when visible.
 */
UCLASS()
class FORGEUI_API UForgeManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()

public:
	UForgeManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	bool Tick(float DeltaTime);
	FTSTicker::FDelegateHandle TickHandle;
};
