#include "HUD/FUProjectedWidget_Frame.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Rendering/DrawElements.h"
#include "SceneView.h"

namespace
{
static void BuildBoundsCorners(const FVector& Origin, const FVector& Extent, TArray<FVector>& OutCorners)
{
	OutCorners.Reset(8);
	for (int32 CornerIndex = 0; CornerIndex < 8; ++CornerIndex)
	{
		OutCorners.Add(FVector(
			Origin.X + ((CornerIndex & 1) != 0 ? Extent.X : -Extent.X),
			Origin.Y + ((CornerIndex & 2) != 0 ? Extent.Y : -Extent.Y),
			Origin.Z + ((CornerIndex & 4) != 0 ? Extent.Z : -Extent.Z)));
	}
}

static bool BuildLocalMeshCorners(const UStaticMesh* StaticMesh, TArray<FVector>& OutLocalCorners)
{
	if (!StaticMesh)
	{
		return false;
	}

	const FBox LocalBounds = StaticMesh->GetBoundingBox();
	if (!LocalBounds.IsValid)
	{
		return false;
	}

	BuildBoundsCorners(LocalBounds.GetCenter(), LocalBounds.GetExtent(), OutLocalCorners);
	return true;
}

static void TransformLocalCornersInline(const USceneComponent* Source, TArray<FVector>& InOutCorners)
{
	const FTransform ComponentTransform = Source->GetComponentTransform();
	for (FVector& Corner : InOutCorners)
	{
		Corner = ComponentTransform.TransformPosition(Corner);
	}
}

static bool ResolveActorWorldCorners(USceneComponent* Source, bool bOnlyCollidingComponents, TArray<FVector>& OutWorldCorners)
{
	AActor* Owner = Source ? Source->GetOwner() : nullptr;
	if (!Owner)
	{
		return false;
	}

	FVector Origin;
	FVector Extent;
	Owner->GetActorBounds(bOnlyCollidingComponents, Origin, Extent, false);
	BuildBoundsCorners(Origin, Extent, OutWorldCorners);
	return true;
}

static bool ResolvePrimitiveWorldCorners(USceneComponent* Source, bool bProjectComponentLocalBounds, TArray<FVector>& OutWorldCorners)
{
	const UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Source);
	if (!PrimitiveComponent)
	{
		return false;
	}

	if (bProjectComponentLocalBounds)
	{
		const FBoxSphereBounds LocalBounds = PrimitiveComponent->GetLocalBounds();
		if (!LocalBounds.ContainsNaN())
		{
			BuildBoundsCorners(LocalBounds.Origin, LocalBounds.BoxExtent, OutWorldCorners);
			TransformLocalCornersInline(Source, OutWorldCorners);
			return true;
		}
	}

	BuildBoundsCorners(PrimitiveComponent->Bounds.Origin, PrimitiveComponent->Bounds.BoxExtent, OutWorldCorners);
	return true;
}

static bool ProjectFrameRect(
	const FFUProjectedWidgetLayoutContext& Context,
	const TArray<FVector>& WorldCorners,
	const FFUProjectedFrameSettings& Settings,
	FVector2D& OutPosition,
	FVector2D& OutSize)
{
	OutPosition = FVector2D::ZeroVector;
	OutSize = FVector2D::ZeroVector;

	if (!Context.PlayerController || Context.CanvasSize.X <= 0.0f || Context.CanvasSize.Y <= 0.0f || WorldCorners.IsEmpty())
	{
		return false;
	}

	ULocalPlayer* LocalPlayer = Context.PlayerController->GetLocalPlayer();
	if (!LocalPlayer || !LocalPlayer->ViewportClient)
	{
		return false;
	}

	FSceneViewProjectionData ProjectionData;
	if (!LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport, ProjectionData))
	{
		return false;
	}

	const FMatrix ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();
	const FIntRect ConstrainedViewRect = ProjectionData.GetConstrainedViewRect();
	const FVector2D ViewRectMin(ConstrainedViewRect.Min.X, ConstrainedViewRect.Min.Y);

	FVector2D MinPosition(TNumericLimits<float>::Max(), TNumericLimits<float>::Max());
	FVector2D MaxPosition(TNumericLimits<float>::Lowest(), TNumericLimits<float>::Lowest());
	bool bHasProjectedCorner = false;

	for (const FVector& Corner : WorldCorners)
	{
		FVector2D ScreenPosition;
		if (!FSceneView::ProjectWorldToScreen(Corner, ConstrainedViewRect, ViewProjectionMatrix, ScreenPosition))
		{
			continue;
		}

		ScreenPosition -= ViewRectMin;

		const FVector2D RoundedScreenPosition(FMath::RoundToDouble(ScreenPosition.X), FMath::RoundToDouble(ScreenPosition.Y));

		FVector2D ViewportLocalPosition;
		USlateBlueprintLibrary::ScreenToViewport(Context.PlayerController, RoundedScreenPosition, ViewportLocalPosition);

		const FVector2D AbsolutePosition = USlateBlueprintLibrary::LocalToAbsolute(Context.ViewportGeometry, ViewportLocalPosition);
		const FVector2D CanvasLocalPosition = USlateBlueprintLibrary::AbsoluteToLocal(Context.CanvasGeometry, AbsolutePosition);

		MinPosition.X = FMath::Min(MinPosition.X, CanvasLocalPosition.X);
		MinPosition.Y = FMath::Min(MinPosition.Y, CanvasLocalPosition.Y);
		MaxPosition.X = FMath::Max(MaxPosition.X, CanvasLocalPosition.X);
		MaxPosition.Y = FMath::Max(MaxPosition.Y, CanvasLocalPosition.Y);
		bHasProjectedCorner = true;
	}

	if (!bHasProjectedCorner)
	{
		return false;
	}

	const float Padding = FMath::Max(0.0f, Settings.Padding);
	MinPosition -= FVector2D(Padding, Padding);
	MaxPosition += FVector2D(Padding, Padding);

	OutSize = MaxPosition - MinPosition;

	const FVector2D MinSize(
		FMath::Max(0.0f, Settings.MinSize.X),
		FMath::Max(0.0f, Settings.MinSize.Y));

	if (OutSize.X < MinSize.X)
	{
		const float Adjustment = (MinSize.X - OutSize.X) * 0.5f;
		MinPosition.X -= Adjustment;
		OutSize.X = MinSize.X;
	}

	if (OutSize.Y < MinSize.Y)
	{
		const float Adjustment = (MinSize.Y - OutSize.Y) * 0.5f;
		MinPosition.Y -= Adjustment;
		OutSize.Y = MinSize.Y;
	}

	OutPosition = MinPosition;
	return OutSize.X > 0.0f && OutSize.Y > 0.0f;
}
}

UFUProjectedWidget_Frame::UFUProjectedWidget_Frame()
{
	bSelfManagedSlotLayout = true;
}

void UFUProjectedWidget_Frame::SetFrameTarget(USceneComponent* Source, int32 ItemIndex)
{
	FrameTargetSource = Source;
	FrameTargetItemIndex = ItemIndex;
	bHasExplicitFrameTarget = Source != nullptr;
	bHasExplicitWorldBounds = false;
}

void UFUProjectedWidget_Frame::SetFrameTargetWorldBounds(const FBox& WorldBounds)
{
	ExplicitWorldBounds = WorldBounds;
	bHasExplicitWorldBounds = true;
}

void UFUProjectedWidget_Frame::ClearFrameTarget()
{
	FrameTargetSource = nullptr;
	FrameTargetItemIndex = INDEX_NONE;
	bHasExplicitFrameTarget = false;
	bHasExplicitWorldBounds = false;
	bHasFrameRect = false;
	FrameSize = FVector2D::ZeroVector;
}

void UFUProjectedWidget_Frame::SetProjectComponentLocalBounds(bool bInProjectComponentLocalBounds)
{
	FrameSettings.bProjectComponentLocalBounds = bInProjectComponentLocalBounds;
}

void UFUProjectedWidget_Frame::NativeOnProjectionLayoutUpdated(const FFUProjectedWidgetLayoutContext& Context)
{
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot);

	if (CanvasSlot && !bSlotInitialized)
	{
		CanvasSlot->SetAutoSize(false);
		CanvasSlot->SetAlignment(FVector2D::ZeroVector);
		SetRenderScale(FVector2D(1.0f, 1.0f));
		SetRenderTransformPivot(FVector2D::ZeroVector);
		bSlotInitialized = true;
	}

	USceneComponent* FrameSource = bHasExplicitFrameTarget ? FrameTargetSource.Get() : ProjectionSource.Get();

	if (!ResolveFrameTargetWorldCorners(FrameSource, WorldCornersScratch))
	{
		bHasFrameRect = false;
		FrameSize = FVector2D::ZeroVector;
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	FVector2D NewFramePosition;
	FVector2D NewFrameSize;
	if (!ProjectFrameRect(Context, WorldCornersScratch, FrameSettings, NewFramePosition, NewFrameSize))
	{
		bHasFrameRect = false;
		FrameSize = FVector2D::ZeroVector;
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const bool bRectUnchanged =
		bHasFrameRect &&
		NewFramePosition.Equals(FramePosition, 0.1f) &&
		NewFrameSize.Equals(FrameSize, 0.1f);

	if (CanvasSlot && !bRectUnchanged)
	{
		CanvasSlot->SetPosition(NewFramePosition);
		CanvasSlot->SetSize(NewFrameSize);
		FramePosition = NewFramePosition;
		FrameSize = NewFrameSize;
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);

	bHasFrameRect = true;
}

int32 UFUProjectedWidget_Frame::NativePaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	int32 MaxLayer = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	const float LineThickness = FMath::Max(0.0f, FrameSettings.LineThickness);
	if (!bHasFrameRect || LineThickness <= 0.0f || FrameSize.X <= 0.0f || FrameSize.Y <= 0.0f)
	{
		return MaxLayer;
	}

	const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
	const float Inset = LineThickness * 0.5f;
	const float Right = FMath::Max(Inset, LocalSize.X - Inset);
	const float Bottom = FMath::Max(Inset, LocalSize.Y - Inset);

	PaintPointsScratch.Reset(5);
	PaintPointsScratch.Add(FVector2D(Inset, Inset));
	PaintPointsScratch.Add(FVector2D(Right, Inset));
	PaintPointsScratch.Add(FVector2D(Right, Bottom));
	PaintPointsScratch.Add(FVector2D(Inset, Bottom));
	PaintPointsScratch.Add(FVector2D(Inset, Inset));

	FSlateDrawElement::MakeLines(
		OutDrawElements,
		++MaxLayer,
		AllottedGeometry.ToPaintGeometry(),
		PaintPointsScratch,
		ESlateDrawEffect::None,
		InWidgetStyle.GetColorAndOpacityTint() * FrameSettings.Tint,
		true,
		LineThickness);

	return MaxLayer;
}

bool UFUProjectedWidget_Frame::ResolveFrameTargetWorldCorners(USceneComponent* FrameSource, TArray<FVector>& OutWorldCorners)
{
	if (bHasExplicitWorldBounds)
	{
		if (!ExplicitWorldBounds.IsValid)
		{
			return false;
		}

		BuildBoundsCorners(ExplicitWorldBounds.GetCenter(), ExplicitWorldBounds.GetExtent(), OutWorldCorners);
		return true;
	}

	if (!FrameSource)
	{
		return false;
	}

	EFUProjectedFrameTargetMode ResolvedTargetMode = FrameSettings.TargetMode;
	if (ResolvedTargetMode == EFUProjectedFrameTargetMode::Auto)
	{
		if (Cast<UInstancedStaticMeshComponent>(FrameSource) && FrameTargetItemIndex >= 0)
		{
			ResolvedTargetMode = EFUProjectedFrameTargetMode::InstancedStaticMeshInstance;
		}
		else if (Cast<UPrimitiveComponent>(FrameSource))
		{
			ResolvedTargetMode = EFUProjectedFrameTargetMode::ComponentBounds;
		}
		else
		{
			ResolvedTargetMode = EFUProjectedFrameTargetMode::OwnerActor;
		}
	}

	switch (ResolvedTargetMode)
	{
	case EFUProjectedFrameTargetMode::InstancedStaticMeshInstance:
		if (ResolveInstancedStaticMeshWorldCorners(FrameSource, OutWorldCorners))
		{
			return true;
		}
		return ResolvePrimitiveWorldCorners(FrameSource, FrameSettings.bProjectComponentLocalBounds, OutWorldCorners) ||
			ResolveActorWorldCorners(FrameSource, FrameSettings.bOnlyCollidingComponents, OutWorldCorners);

	case EFUProjectedFrameTargetMode::ComponentBounds:
		return ResolvePrimitiveWorldCorners(FrameSource, FrameSettings.bProjectComponentLocalBounds, OutWorldCorners) ||
			ResolveActorWorldCorners(FrameSource, FrameSettings.bOnlyCollidingComponents, OutWorldCorners);

	case EFUProjectedFrameTargetMode::OwnerActor:
	default:
		return ResolveActorWorldCorners(FrameSource, FrameSettings.bOnlyCollidingComponents, OutWorldCorners);
	}
}

bool UFUProjectedWidget_Frame::ResolveInstancedStaticMeshWorldCorners(USceneComponent* FrameSource, TArray<FVector>& OutWorldCorners)
{
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent = Cast<UInstancedStaticMeshComponent>(FrameSource);
	if (!InstancedStaticMeshComponent || FrameTargetItemIndex < 0 || FrameTargetItemIndex >= InstancedStaticMeshComponent->GetInstanceCount())
	{
		return false;
	}

	UStaticMesh* StaticMesh = InstancedStaticMeshComponent->GetStaticMesh();
	if (!StaticMesh)
	{
		return false;
	}

	if (CachedLocalBoundsMesh.Get() != StaticMesh || CachedLocalBoundsCorners.Num() != 8)
	{
		if (!BuildLocalMeshCorners(StaticMesh, CachedLocalBoundsCorners))
		{
			CachedLocalBoundsMesh = nullptr;
			return false;
		}

		CachedLocalBoundsMesh = StaticMesh;
	}

	FTransform InstanceTransform;
	if (!InstancedStaticMeshComponent->GetInstanceTransform(FrameTargetItemIndex, InstanceTransform, true))
	{
		return false;
	}

	OutWorldCorners.Reset(CachedLocalBoundsCorners.Num());
	for (const FVector& LocalCorner : CachedLocalBoundsCorners)
	{
		OutWorldCorners.Add(InstanceTransform.TransformPosition(LocalCorner));
	}

	return !OutWorldCorners.IsEmpty();
}
