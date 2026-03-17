#pragma once

#include "CoreMinimal.h"
#include "GameUIPolicy.h"
#include "ForgeUIPolicy.generated.h"

/**
 * ForgeUI policy subclass that safely overrides GetWorld() to prevent
 * CastChecked crash when editing the CDO in the editor (where the Outer
 * is the package, not a UGameUIManagerSubsystem).
 */
UCLASS()
class FORGEUI_API UForgeUIPolicy : public UGameUIPolicy
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;
};
