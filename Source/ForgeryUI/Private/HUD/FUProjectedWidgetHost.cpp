#include "HUD/FUProjectedWidgetHost.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SceneComponent.h"
#include "HUD/FUProjectedWidget_Frame.h"
#include "HUD/FUScreenProjectedWidget.h"

namespace
{
static FFUProjectedWidgetSettings SanitizeSettings(const FFUProjectedWidgetSettings& InSettings)
{
	FFUProjectedWidgetSettings Settings = InSettings;

	Settings.Visibility.ClampMargin = FMath::Max(0.0f, Settings.Visibility.ClampMargin);
	Settings.Visibility.MinVisibleDistance = FMath::Max(0.0f, Settings.Visibility.MinVisibleDistance);
	Settings.Visibility.NearDistance = FMath::Max(Settings.Visibility.MinVisibleDistance, Settings.Visibility.NearDistance);
	Settings.Visibility.FarDistance = FMath::Max(Settings.Visibility.NearDistance, Settings.Visibility.FarDistance);
	Settings.Visibility.MaxVisibleDistance = FMath::Max(Settings.Visibility.FarDistance, Settings.Visibility.MaxVisibleDistance);

	Settings.Scale.ConstantScale = FMath::Max(0.0f, Settings.Scale.ConstantScale);
	Settings.Scale.ReferenceDistance = FMath::Max(1.0f, Settings.Scale.ReferenceDistance);
	Settings.Scale.ReferenceScale = FMath::Max(0.0f, Settings.Scale.ReferenceScale);
	Settings.Scale.MinScale = FMath::Max(0.0f, Settings.Scale.MinScale);
	Settings.Scale.MaxScale = FMath::Max(Settings.Scale.MinScale, Settings.Scale.MaxScale);
	Settings.Scale.NearScale = FMath::Max(0.0f, Settings.Scale.NearScale);
	Settings.Scale.NormalScale = FMath::Max(0.0f, Settings.Scale.NormalScale);
	Settings.Scale.FarScale = FMath::Max(0.0f, Settings.Scale.FarScale);
	return Settings;
}

static EFUProjectedWidgetDistanceState ResolveDistanceState(float DistanceToCamera, float MinVisibleDistance, float NearDistance, float FarDistance, float MaxVisibleDistance)
{
	if (DistanceToCamera < MinVisibleDistance)
	{
		return EFUProjectedWidgetDistanceState::HiddenTooClose;
	}

	if (DistanceToCamera > MaxVisibleDistance)
	{
		return EFUProjectedWidgetDistanceState::HiddenTooFar;
	}

	if (DistanceToCamera <= NearDistance)
	{
		return EFUProjectedWidgetDistanceState::Near;
	}

	if (DistanceToCamera >= FarDistance)
	{
		return EFUProjectedWidgetDistanceState::Far;
	}

	return EFUProjectedWidgetDistanceState::Normal;
}

static float ResolveScale(
	float DistanceToCamera,
	EFUProjectedWidgetDistanceState DistanceState,
	EFUProjectedWidgetScaleMode ScaleMode,
	float ConstantScale,
	float ReferenceDistance,
	float ReferenceScale,
	float MinScale,
	float MaxScale,
	float NearScale,
	float NormalScale,
	float FarScale)
{
	switch (ScaleMode)
	{
	case EFUProjectedWidgetScaleMode::Constant:
		return ConstantScale;

	case EFUProjectedWidgetScaleMode::Proportional:
		{
			const float SafeDistance = FMath::Max(DistanceToCamera, 1.0f);
			const float ProportionalScale = ReferenceScale * (ReferenceDistance / SafeDistance);
			return FMath::Clamp(ProportionalScale, MinScale, MaxScale);
		}

	case EFUProjectedWidgetScaleMode::Custom:
		switch (DistanceState)
		{
		case EFUProjectedWidgetDistanceState::Near:
			return NearScale;

		case EFUProjectedWidgetDistanceState::Far:
			return FarScale;

		case EFUProjectedWidgetDistanceState::Normal:
		default:
			return NormalScale;
		}

	default:
		return ConstantScale;
	}
}
}

void UFUProjectedWidgetHost::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

UFUScreenProjectedWidget* UFUProjectedWidgetHost::RegisterWidget(
	USceneComponent* Anchor,
	TSubclassOf<UFUScreenProjectedWidget> WidgetClass,
	FVector AnchorLocalOffset,
	FVector2D ScreenOffset,
	int32 ZOrder)
{
	if (!Anchor || !MarkerCanvas || !WidgetClass)
	{
		return nullptr;
	}

	for (int32 Index = Anchors.Num() - 1; Index >= 0; --Index)
	{
		USceneComponent* ExistingAnchor = Anchors[Index].Get();
		UFUScreenProjectedWidget* ExistingWidget = Widgets.IsValidIndex(Index) ? Widgets[Index] : nullptr;

		if (!ExistingAnchor || !IsValid(ExistingWidget))
		{
			RemoveEntryAt(Index);
			continue;
		}

		if (ExistingAnchor == Anchor && WidgetClasses.IsValidIndex(Index) && WidgetClasses[Index] == WidgetClass)
		{
			AnchorLocalOffsets[Index] = AnchorLocalOffset;
			AnchorPositions[Index] = Anchor->GetComponentTransform().TransformPosition(AnchorLocalOffset);
			ScreenOffsets[Index] = ScreenOffset;

			if (ZOrders[Index] != ZOrder)
			{
				ZOrders[Index] = ZOrder;
				if (UCanvasPanelSlot* ChildSlot = Cast<UCanvasPanelSlot>(ExistingWidget->Slot))
				{
					ChildSlot->SetZOrder(ZOrder);
				}
			}

			return ExistingWidget;
		}
	}

	UFUScreenProjectedWidget* Widget = CreateWidget<UFUScreenProjectedWidget>(GetOwningPlayer(), WidgetClass);

	if (!Widget)
	{
		return nullptr;
	}

	Widget->SetProjectionSource(Anchor);
	Widget->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (UCanvasPanelSlot* ChildSlot = Cast<UCanvasPanelSlot>(MarkerCanvas->AddChild(Widget)))
	{
		ChildSlot->SetAnchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f));
		ChildSlot->SetAutoSize(true);
		ChildSlot->SetZOrder(ZOrder);
	}

	const int32 Index = Anchors.Add(Anchor);
	AnchorPositions.Add(Anchor->GetComponentTransform().TransformPosition(AnchorLocalOffset));
	AnchorLocalOffsets.Add(AnchorLocalOffset);
	Widgets.Add(Widget);
	WidgetClasses.Add(WidgetClass);

	ScreenOffsets.Add(ScreenOffset);
	Pivots.Add(FVector2D(0.5f, 0.5f));
	ZOrders.Add(ZOrder);
	OffscreenModes.Add(EFUProjectedWidgetOffscreenMode::Hide);
	ClampMargins.Add(12.0f);
	MinVisibleDistances.Add(0.0f);
	NearDistances.Add(1000.0f);
	FarDistances.Add(3000.0f);
	MaxVisibleDistances.Add(10000.0f);
	ScaleModes.Add(EFUProjectedWidgetScaleMode::Constant);
	ConstantScales.Add(1.0f);
	ReferenceDistances.Add(1500.0f);
	ReferenceScales.Add(1.0f);
	MinScales.Add(0.5f);
	MaxScales.Add(1.5f);
	NearScales.Add(1.25f);
	NormalScales.Add(1.0f);
	FarScales.Add(0.75f);
	DistanceStates.Add(EFUProjectedWidgetDistanceState::Normal);
	ResolvedScales.Add(1.0f);
	ClampedStates.Add(false);
	InitializedStates.Add(false);

	ApplySettingsAt(Index, Widget->Settings);
	return Widget;
}

void UFUProjectedWidgetHost::UnregisterWidget(USceneComponent* Anchor)
{
	if (!Anchor)
	{
		return;
	}

	for (int32 Index = Anchors.Num() - 1; Index >= 0; --Index)
	{
		if (Anchors[Index].Get() == Anchor)
		{
			RemoveEntryAt(Index);
		}
	}
}

void UFUProjectedWidgetHost::UnregisterWidgetInstance(UFUScreenProjectedWidget* Widget)
{
	if (!Widget)
	{
		return;
	}

	for (int32 Index = Widgets.Num() - 1; Index >= 0; --Index)
	{
		if (Widgets[Index] == Widget)
		{
			RemoveEntryAt(Index);
			return;
		}
	}
}

UFUProjectedWidget_Frame* UFUProjectedWidgetHost::RegisterFrameWidget(
	USceneComponent* Anchor,
	TSubclassOf<UFUProjectedWidget_Frame> WidgetClass,
	FVector AnchorLocalOffset,
	FVector2D ScreenOffset,
	int32 ZOrder)
{
	if (!WidgetClass)
	{
		WidgetClass = UFUProjectedWidget_Frame::StaticClass();
	}

	TSubclassOf<UFUScreenProjectedWidget> ScreenWidgetClass = WidgetClass.Get();
	return Cast<UFUProjectedWidget_Frame>(RegisterWidget(Anchor, ScreenWidgetClass, AnchorLocalOffset, ScreenOffset, ZOrder));
}

void UFUProjectedWidgetHost::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsDesignTime() || !MarkerCanvas)
	{
		return;
	}

	APlayerController* PlayerController = GetOwningPlayer();

	if (!PlayerController || !PlayerController->PlayerCameraManager)
	{
		return;
	}

	for (int32 Index = Anchors.Num() - 1; Index >= 0; --Index)
	{
		USceneComponent* Anchor = Anchors[Index].Get();
		UFUScreenProjectedWidget* Widget = Widgets.IsValidIndex(Index) ? Widgets[Index] : nullptr;

		if (!Anchor || !IsValid(Widget))
		{
			RemoveEntryAt(Index);
			continue;
		}

		AnchorPositions[Index] = Anchor->GetComponentTransform().TransformPosition(AnchorLocalOffsets[Index]);
	}

	const FGeometry CanvasGeometry = MarkerCanvas->GetCachedGeometry();
	const FVector2D CanvasSize = CanvasGeometry.GetLocalSize();

	if (CanvasSize.X <= 0.0f || CanvasSize.Y <= 0.0f)
	{
		return;
	}

	const FGeometry ViewportGeometry = UWidgetLayoutLibrary::GetPlayerScreenWidgetGeometry(PlayerController);
	const FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
	const FFUProjectedWidgetLayoutContext LayoutContext(
		PlayerController,
		MarkerCanvas,
		ViewportGeometry,
		CanvasGeometry,
		CanvasSize);

	for (int32 Index = 0; Index < Anchors.Num(); ++Index)
	{
		UFUScreenProjectedWidget* Widget = Widgets[Index];
		const FVector AnchorPosition = AnchorPositions[Index];
		const float DistanceToCamera = FVector::Distance(CameraLocation, AnchorPosition);
		const EFUProjectedWidgetDistanceState DistanceState = ResolveDistanceState(
			DistanceToCamera,
			MinVisibleDistances[Index],
			NearDistances[Index],
			FarDistances[Index],
			MaxVisibleDistances[Index]);
		const bool bVisibleByDistance =
			DistanceState != EFUProjectedWidgetDistanceState::HiddenTooClose &&
			DistanceState != EFUProjectedWidgetDistanceState::HiddenTooFar;
		const float ResolvedScale = ResolveScale(
			DistanceToCamera,
			DistanceState,
			ScaleModes[Index],
			ConstantScales[Index],
			ReferenceDistances[Index],
			ReferenceScales[Index],
			MinScales[Index],
			MaxScales[Index],
			NearScales[Index],
			NormalScales[Index],
			FarScales[Index]);

		const bool bWasInitialized = InitializedStates[Index] != 0;
		const EFUProjectedWidgetDistanceState PreviousDistanceState = DistanceStates[Index];
		const bool bPreviousClamped = ClampedStates[Index] != 0;

		const bool bSelfManagedSlotLayout = Widget->UsesSelfManagedSlotLayout();

		Widget->SetResolvedProjectionScale(ResolvedScale);
		if (!bSelfManagedSlotLayout)
		{
			Widget->SetRenderScale(FVector2D(ResolvedScale, ResolvedScale));
		}
		ResolvedScales[Index] = ResolvedScale;

		const bool bDistanceStateChanged = !bWasInitialized || PreviousDistanceState != DistanceState;
		DistanceStates[Index] = DistanceState;
		Widget->SetDistanceState(DistanceState, bDistanceStateChanged);

		if (!bVisibleByDistance)
		{
			const bool bClampedChanged = !bWasInitialized || bPreviousClamped;
			ClampedStates[Index] = false;
			Widget->SetProjectionClamped(false, bClampedChanged);
			Widget->SetVisibility(ESlateVisibility::Collapsed);
			InitializedStates[Index] = true;
			continue;
		}

		FVector2D ViewportLocalPosition;
		if (!UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PlayerController, AnchorPosition, ViewportLocalPosition, true))
		{
			const bool bClampedChanged = !bWasInitialized || bPreviousClamped;
			ClampedStates[Index] = false;
			Widget->SetProjectionClamped(false, bClampedChanged);
			Widget->SetVisibility(ESlateVisibility::Collapsed);
			InitializedStates[Index] = true;
			continue;
		}

		const FVector2D AbsolutePosition = USlateBlueprintLibrary::LocalToAbsolute(ViewportGeometry, ViewportLocalPosition);
		const FVector2D LocalPosition = USlateBlueprintLibrary::AbsoluteToLocal(CanvasGeometry, AbsolutePosition + ScreenOffsets[Index]);
		const bool bInsideCanvas =
			LocalPosition.X >= 0.0f &&
			LocalPosition.X <= CanvasSize.X &&
			LocalPosition.Y >= 0.0f &&
			LocalPosition.Y <= CanvasSize.Y;

		FVector2D DrawPosition = LocalPosition;
		bool bClamped = false;

		if (!bInsideCanvas)
		{
			if (OffscreenModes[Index] == EFUProjectedWidgetOffscreenMode::Hide)
			{
				const bool bClampedChanged = !bWasInitialized || bPreviousClamped;
				ClampedStates[Index] = false;
				Widget->SetProjectionClamped(false, bClampedChanged);
				Widget->SetVisibility(ESlateVisibility::Collapsed);
				InitializedStates[Index] = true;
				continue;
			}

			bClamped = true;
			const float Margin = ClampMargins[Index];
			const float MaxX = FMath::Max(Margin, CanvasSize.X - Margin);
			const float MaxY = FMath::Max(Margin, CanvasSize.Y - Margin);
			DrawPosition.X = FMath::Clamp(DrawPosition.X, Margin, MaxX);
			DrawPosition.Y = FMath::Clamp(DrawPosition.Y, Margin, MaxY);
		}

		const bool bClampedChanged = !bWasInitialized || bPreviousClamped != bClamped;
		ClampedStates[Index] = bClamped;
		Widget->SetProjectionClamped(bClamped, bClampedChanged);

		if (!bSelfManagedSlotLayout)
		{
			if (UCanvasPanelSlot* ChildSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
			{
				ChildSlot->SetAlignment(Pivots[Index]);
				ChildSlot->SetPosition(DrawPosition);
				ChildSlot->SetZOrder(ZOrders[Index]);
			}
		}

		Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
		InitializedStates[Index] = true;
		Widget->NotifyProjectionLayoutUpdated(LayoutContext);
	}
}

void UFUProjectedWidgetHost::RemoveEntryAt(int32 Index)
{
	if (!Anchors.IsValidIndex(Index))
	{
		return;
	}

	if (Widgets.IsValidIndex(Index) && IsValid(Widgets[Index]))
	{
		Widgets[Index]->RemoveFromParent();
	}

	Anchors.RemoveAtSwap(Index);
	AnchorPositions.RemoveAtSwap(Index);
	AnchorLocalOffsets.RemoveAtSwap(Index);
	Widgets.RemoveAtSwap(Index);
	WidgetClasses.RemoveAtSwap(Index);
	ScreenOffsets.RemoveAtSwap(Index);
	Pivots.RemoveAtSwap(Index);
	ZOrders.RemoveAtSwap(Index);
	OffscreenModes.RemoveAtSwap(Index);
	ClampMargins.RemoveAtSwap(Index);
	MinVisibleDistances.RemoveAtSwap(Index);
	NearDistances.RemoveAtSwap(Index);
	FarDistances.RemoveAtSwap(Index);
	MaxVisibleDistances.RemoveAtSwap(Index);
	ScaleModes.RemoveAtSwap(Index);
	ConstantScales.RemoveAtSwap(Index);
	ReferenceDistances.RemoveAtSwap(Index);
	ReferenceScales.RemoveAtSwap(Index);
	MinScales.RemoveAtSwap(Index);
	MaxScales.RemoveAtSwap(Index);
	NearScales.RemoveAtSwap(Index);
	NormalScales.RemoveAtSwap(Index);
	FarScales.RemoveAtSwap(Index);
	DistanceStates.RemoveAtSwap(Index);
	ResolvedScales.RemoveAtSwap(Index);
	ClampedStates.RemoveAtSwap(Index);
	InitializedStates.RemoveAtSwap(Index);
}

void UFUProjectedWidgetHost::ApplySettingsAt(int32 Index, const FFUProjectedWidgetSettings& InSettings)
{
	if (!Anchors.IsValidIndex(Index))
	{
		return;
	}

	const FFUProjectedWidgetSettings Settings = SanitizeSettings(InSettings);
	Pivots[Index] = Settings.Layout.Pivot;
	OffscreenModes[Index] = Settings.Visibility.OffscreenMode;
	ClampMargins[Index] = Settings.Visibility.ClampMargin;
	MinVisibleDistances[Index] = Settings.Visibility.MinVisibleDistance;
	NearDistances[Index] = Settings.Visibility.NearDistance;
	FarDistances[Index] = Settings.Visibility.FarDistance;
	MaxVisibleDistances[Index] = Settings.Visibility.MaxVisibleDistance;
	ScaleModes[Index] = Settings.Scale.ScaleMode;
	ConstantScales[Index] = Settings.Scale.ConstantScale;
	ReferenceDistances[Index] = Settings.Scale.ReferenceDistance;
	ReferenceScales[Index] = Settings.Scale.ReferenceScale;
	MinScales[Index] = Settings.Scale.MinScale;
	MaxScales[Index] = Settings.Scale.MaxScale;
	NearScales[Index] = Settings.Scale.NearScale;
	NormalScales[Index] = Settings.Scale.NormalScale;
	FarScales[Index] = Settings.Scale.FarScale;
}
