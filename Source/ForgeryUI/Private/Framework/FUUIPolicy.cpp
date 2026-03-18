#include "Framework/FUUIPolicy.h"

#include "GameUIManagerSubsystem.h"

UWorld* UFUUIPolicy::GetWorld() const
{
	if (const UGameUIManagerSubsystem* UIManager = Cast<UGameUIManagerSubsystem>(GetOuter()))
	{
		return UIManager->GetGameInstance()->GetWorld();
	}
	return nullptr;
}
