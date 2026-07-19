#include "HUD/FUHudWorldLineWidget.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SceneComponent.h"
#include "Components/Widget.h"
#include "GameFramework/PlayerController.h"
#include "Rendering/DrawElements.h"

void UFUHudWorldLineWidget::SetWorldTargetLocation(FVector WorldLocation)
{
	TargetComponent = nullptr;
	ExplicitWorldTargetLocation = WorldLocation;
	bHasExplicitWorldTarget = true;
}

void UFUHudWorldLineWidget::SetWorldTargetComponent(USceneComponent* Component, FVector LocalOffset)
{
	TargetComponent = Component;
	TargetLocalOffset = LocalOffset;
	bHasExplicitWorldTarget = false;
}

void UFUHudWorldLineWidget::ClearWorldTarget()
{
	TargetComponent = nullptr;
	TargetLocalOffset = FVector::ZeroVector;
	ExplicitWorldTargetLocation = FVector::ZeroVector;
	bHasExplicitWorldTarget = false;
}

void UFUHudWorldLineWidget::SetStartLocalPosition(FVector2D LocalPosition)
{
	StartLocalPosition = LocalPosition;
}

void UFUHudWorldLineWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

int32 UFUHudWorldLineWidget::NativePaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	int32 MaxLayer = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	const float ResolvedLineThickness = FMath::Max(0.0f, LineThickness);
	if (ResolvedLineThickness <= 0.0f)
	{
		return MaxLayer;
	}

	FVector WorldLocation;
	if (!ResolveWorldTarget(WorldLocation))
	{
		return MaxLayer;
	}

	FVector2D StartPosition;
	if (!ResolveStartPosition(AllottedGeometry, StartPosition))
	{
		return MaxLayer;
	}

	FVector2D EndPosition;
	if (!ProjectWorldTarget(AllottedGeometry, WorldLocation, EndPosition))
	{
		if (bHideWhenProjectionFails || !bHasLastProjectedEndPosition)
		{
			return MaxLayer;
		}

		EndPosition = LastProjectedEndPosition;
	}
	else
	{
		LastProjectedEndPosition = EndPosition;
		bHasLastProjectedEndPosition = true;
	}

	PaintPointsScratch.Reset(2);
	PaintPointsScratch.Add(StartPosition);
	PaintPointsScratch.Add(EndPosition);

	FSlateDrawElement::MakeLines(
		OutDrawElements,
		++MaxLayer,
		AllottedGeometry.ToPaintGeometry(),
		PaintPointsScratch,
		ESlateDrawEffect::None,
		InWidgetStyle.GetColorAndOpacityTint() * Tint,
		true,
		ResolvedLineThickness);

	return MaxLayer;
}

bool UFUHudWorldLineWidget::ResolveWorldTarget(FVector& OutWorldLocation) const
{
	if (const USceneComponent* Component = TargetComponent.Get())
	{
		OutWorldLocation = Component->GetComponentTransform().TransformPosition(TargetLocalOffset);
		return true;
	}

	if (bHasExplicitWorldTarget)
	{
		OutWorldLocation = ExplicitWorldTargetLocation;
		return true;
	}

	return false;
}

bool UFUHudWorldLineWidget::ResolveStartPosition(const FGeometry& AllottedGeometry, FVector2D& OutStartPosition) const
{
	if (StartAnchor)
	{
		const FGeometry AnchorGeometry = StartAnchor->GetCachedGeometry();
		const FVector2D AnchorSize = AnchorGeometry.GetLocalSize();
		if (AnchorSize.X > 0.0f || AnchorSize.Y > 0.0f)
		{
			const FVector2D AnchorCenter = AnchorSize * 0.5f;
			const FVector2D AbsolutePosition = USlateBlueprintLibrary::LocalToAbsolute(AnchorGeometry, AnchorCenter);
			OutStartPosition = USlateBlueprintLibrary::AbsoluteToLocal(AllottedGeometry, AbsolutePosition);
			return true;
		}
	}

	OutStartPosition = StartLocalPosition;
	return true;
}

bool UFUHudWorldLineWidget::ProjectWorldTarget(const FGeometry& AllottedGeometry, const FVector& WorldLocation, FVector2D& OutEndPosition) const
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (!PlayerController)
	{
		return false;
	}

	FVector2D ViewportLocalPosition;
	if (!UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PlayerController, WorldLocation, ViewportLocalPosition, true))
	{
		return false;
	}

	const FGeometry ViewportGeometry = UWidgetLayoutLibrary::GetPlayerScreenWidgetGeometry(PlayerController);
	const FVector2D AbsolutePosition = USlateBlueprintLibrary::LocalToAbsolute(ViewportGeometry, ViewportLocalPosition);
	OutEndPosition = USlateBlueprintLibrary::AbsoluteToLocal(AllottedGeometry, AbsolutePosition + EndScreenOffset);
	return true;
}
