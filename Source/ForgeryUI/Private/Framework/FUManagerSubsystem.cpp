#include "Framework/FUManagerSubsystem.h"

#include "CommonLocalPlayer.h"
#include "PrimaryGameLayout.h"
#include "GameFramework/HUD.h"
#include "GameUIPolicy.h"

UFUManagerSubsystem::UFUManagerSubsystem()
{
}

void UFUManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UFUManagerSubsystem::Tick), 0.0f);
}

void UFUManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
}

bool UFUManagerSubsystem::Tick(float DeltaTime)
{
	const UGameUIPolicy* Policy = GetCurrentUIPolicy();

	if (!Policy)
	{
		return true;
	}

	const auto LocalPlayer = GetGameInstance()->GetLocalPlayerByIndex(0);

	if (!LocalPlayer)
	{
		return true;
	}

	bool bShouldShowUI = true;

	if (const APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld()))
	{
		const AHUD* HUD = PlayerController->GetHUD();

		if (HUD && !HUD->bShowHUD)
		{
			bShouldShowUI = false;
		}
	}

	if (UPrimaryGameLayout* RootLayout = Policy->GetRootLayout(CastChecked<UCommonLocalPlayer>(LocalPlayer)))
	{
		const ESlateVisibility DesiredVisibility = bShouldShowUI ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;
		if (DesiredVisibility != RootLayout->GetVisibility())
		{
			RootLayout->SetVisibility(DesiredVisibility);
		}
	}

	return true;
}
