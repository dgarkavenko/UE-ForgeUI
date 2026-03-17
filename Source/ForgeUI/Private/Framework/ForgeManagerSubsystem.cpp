#include "Framework/ForgeManagerSubsystem.h"

#include "CommonLocalPlayer.h"
#include "PrimaryGameLayout.h"
#include "GameFramework/HUD.h"
#include "GameUIPolicy.h"

UForgeManagerSubsystem::UForgeManagerSubsystem()
{
}

void UForgeManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UForgeManagerSubsystem::Tick), 0.0f);
}

void UForgeManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
}

bool UForgeManagerSubsystem::Tick(float DeltaTime)
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
