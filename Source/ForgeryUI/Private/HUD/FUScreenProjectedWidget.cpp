#include "HUD/FUScreenProjectedWidget.h"

void UFUScreenProjectedWidget::SetProjectionSource(USceneComponent* InProjectionSource)
{
	ProjectionSource = InProjectionSource;
	ReceiveOnProjectionSourceSet(InProjectionSource);
}

USceneComponent* UFUScreenProjectedWidget::GetProjectionSource() const
{
	return ProjectionSource;
}

float UFUScreenProjectedWidget::GetResolvedProjectionScale() const
{
	return ResolvedProjectionScale;
}

bool UFUScreenProjectedWidget::IsProjectionClamped() const
{
	return bProjectionClamped;
}

EFUProjectedWidgetDistanceState UFUScreenProjectedWidget::GetDistanceState() const
{
	return DistanceState;
}

void UFUScreenProjectedWidget::SetResolvedProjectionScale(float InResolvedProjectionScale)
{
	ResolvedProjectionScale = InResolvedProjectionScale;
}

void UFUScreenProjectedWidget::SetProjectionClamped(bool bInProjectionClamped, bool bFireEvent)
{
	bProjectionClamped = bInProjectionClamped;

	if (bFireEvent)
	{
		ReceiveOnClampedChanged(bInProjectionClamped);
	}
}

void UFUScreenProjectedWidget::SetDistanceState(EFUProjectedWidgetDistanceState InDistanceState, bool bFireEvent)
{
	DistanceState = InDistanceState;

	if (bFireEvent)
	{
		ReceiveOnDistanceStateChanged(InDistanceState);
	}
}

void UFUScreenProjectedWidget::NotifyProjectionLayoutUpdated(const FFUProjectedWidgetLayoutContext& Context)
{
	NativeOnProjectionLayoutUpdated(Context);
	ReceiveOnProjectionLayoutUpdated(Context.PlayerController, Context.ProjectionCanvas);
}

void UFUScreenProjectedWidget::NativeOnProjectionLayoutUpdated(const FFUProjectedWidgetLayoutContext& Context)
{
}
