#include "Framework/ForgeUIPolicy.h"

#include "GameUIManagerSubsystem.h"

UWorld* UForgeUIPolicy::GetWorld() const
{
	if (const UGameUIManagerSubsystem* UIManager = Cast<UGameUIManagerSubsystem>(GetOuter()))
	{
		return UIManager->GetGameInstance()->GetWorld();
	}
	return nullptr;
}
