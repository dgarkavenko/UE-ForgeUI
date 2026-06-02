#pragma once

#include "CoreMinimal.h"
#include "HUD/FUScreenProjectedWidget.h"
#include "FUProjectedWidget_Frame.generated.h"

class UStaticMesh;

UENUM(BlueprintType)
enum class EFUProjectedFrameTargetMode : uint8
{
	Auto,
	OwnerActor,
	ComponentBounds,
	InstancedStaticMeshInstance
};

USTRUCT(BlueprintType)
struct FFUProjectedFrameSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Frame")
	EFUProjectedFrameTargetMode TargetMode = EFUProjectedFrameTargetMode::Auto;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Frame", meta = (ClampMin = "0.0"))
	float Padding = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Frame", meta = (ClampMin = "0.0"))
	FVector2D MinSize = FVector2D(12.0f, 12.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Frame", meta = (ClampMin = "0.0"))
	float LineThickness = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Frame")
	FLinearColor Tint = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Frame")
	bool bProjectComponentLocalBounds = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Frame")
	bool bOnlyCollidingComponents = false;
};

UCLASS(BlueprintType, Blueprintable)
class FORGERYUI_API UFUProjectedWidget_Frame : public UFUScreenProjectedWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, ScriptCallable, Category = "Projection|Frame")
	void SetFrameTarget(USceneComponent* Source, int32 ItemIndex = -1);

	UFUNCTION(BlueprintCallable, ScriptCallable, Category = "Projection|Frame")
	void ClearFrameTarget();

	UFUNCTION(BlueprintCallable, ScriptCallable, Category = "Projection|Frame")
	void SetProjectComponentLocalBounds(bool bInProjectComponentLocalBounds);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Frame")
	FFUProjectedFrameSettings FrameSettings;

protected:
	virtual void NativeOnProjectionLayoutUpdated(const FFUProjectedWidgetLayoutContext& Context) override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

private:
	bool ResolveFrameTargetWorldCorners(USceneComponent* FrameSource, TArray<FVector>& OutWorldCorners);
	bool ResolveInstancedStaticMeshWorldCorners(USceneComponent* FrameSource, TArray<FVector>& OutWorldCorners);

	bool bHasFrameRect = false;
	bool bHasExplicitFrameTarget = false;
	int32 FrameTargetItemIndex = INDEX_NONE;
	FVector2D FrameSize = FVector2D::ZeroVector;

	UPROPERTY(Transient)
	TWeakObjectPtr<USceneComponent> FrameTargetSource;

	TWeakObjectPtr<UStaticMesh> CachedLocalBoundsMesh;
	TArray<FVector> CachedLocalBoundsCorners;
};

// Future batch path: UFUProjectedFrameLayer should draw many FFUProjectedFrameTarget entries
// in one widget/paint pass for enemy groups, sensors, debug overlays, and multi-selection.
