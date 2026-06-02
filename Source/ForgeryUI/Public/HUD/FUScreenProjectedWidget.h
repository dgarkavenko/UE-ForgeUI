#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Layout/Geometry.h"
#include "FUScreenProjectedWidget.generated.h"

class APlayerController;
class USceneComponent;
class UWidget;

struct FFUProjectedWidgetLayoutContext
{
	FFUProjectedWidgetLayoutContext(
		APlayerController* InPlayerController,
		UWidget* InProjectionCanvas,
		const FGeometry& InViewportGeometry,
		const FGeometry& InCanvasGeometry,
		FVector2D InCanvasSize)
		: PlayerController(InPlayerController)
		, ProjectionCanvas(InProjectionCanvas)
		, ViewportGeometry(InViewportGeometry)
		, CanvasGeometry(InCanvasGeometry)
		, CanvasSize(InCanvasSize)
	{
	}

	APlayerController* PlayerController;
	UWidget* ProjectionCanvas;
	FGeometry ViewportGeometry;
	FGeometry CanvasGeometry;
	FVector2D CanvasSize;
};

UENUM(BlueprintType)
enum class EFUProjectedWidgetDistanceState : uint8
{
	HiddenTooClose,
	Near,
	Normal,
	Far,
	HiddenTooFar
};

UENUM(BlueprintType)
enum class EFUProjectedWidgetOffscreenMode : uint8
{
	Hide,
	ClampToViewport
};

UENUM(BlueprintType)
enum class EFUProjectedWidgetScaleMode : uint8
{
	Constant,
	Proportional,
	Custom
};

USTRUCT(BlueprintType)
struct FFUProjectedWidgetLayoutSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Layout")
	FVector2D Pivot = FVector2D(0.5f, 0.5f);
};

USTRUCT(BlueprintType)
struct FFUProjectedWidgetVisibilitySettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Visibility")
	EFUProjectedWidgetOffscreenMode OffscreenMode = EFUProjectedWidgetOffscreenMode::Hide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Visibility", meta = (ClampMin = "0.0"))
	float ClampMargin = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Visibility", meta = (ClampMin = "0.0", Units = "cm"))
	float MinVisibleDistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Visibility", meta = (ClampMin = "0.0", Units = "cm"))
	float NearDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Visibility", meta = (ClampMin = "0.0", Units = "cm"))
	float FarDistance = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Visibility", meta = (ClampMin = "0.0", Units = "cm"))
	float MaxVisibleDistance = 10000.0f;
};

USTRUCT(BlueprintType)
struct FFUProjectedWidgetScaleSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Scale")
	EFUProjectedWidgetScaleMode ScaleMode = EFUProjectedWidgetScaleMode::Constant;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Scale", meta = (ClampMin = "0.0", EditCondition = "ScaleMode == EFUProjectedWidgetScaleMode::Constant", EditConditionHides))
	float ConstantScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Scale", meta = (ClampMin = "1.0", Units = "cm", EditCondition = "ScaleMode == EFUProjectedWidgetScaleMode::Proportional", EditConditionHides))
	float ReferenceDistance = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Scale", meta = (ClampMin = "0.0", EditCondition = "ScaleMode == EFUProjectedWidgetScaleMode::Proportional", EditConditionHides))
	float ReferenceScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Scale", meta = (ClampMin = "0.0", EditCondition = "ScaleMode == EFUProjectedWidgetScaleMode::Proportional", EditConditionHides))
	float MinScale = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Scale", meta = (ClampMin = "0.0", EditCondition = "ScaleMode == EFUProjectedWidgetScaleMode::Proportional", EditConditionHides))
	float MaxScale = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Scale", meta = (ClampMin = "0.0", EditCondition = "ScaleMode == EFUProjectedWidgetScaleMode::Custom", EditConditionHides))
	float NearScale = 1.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Scale", meta = (ClampMin = "0.0", EditCondition = "ScaleMode == EFUProjectedWidgetScaleMode::Custom", EditConditionHides))
	float NormalScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Scale", meta = (ClampMin = "0.0", EditCondition = "ScaleMode == EFUProjectedWidgetScaleMode::Custom", EditConditionHides))
	float FarScale = 0.75f;
};

USTRUCT(BlueprintType)
struct FFUProjectedWidgetSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection")
	FFUProjectedWidgetLayoutSettings Layout;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection")
	FFUProjectedWidgetVisibilitySettings Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection")
	FFUProjectedWidgetScaleSettings Scale;
};

/**
 * Lightweight base class for widgets that are projected from a world-space anchor.
 */
UCLASS(BlueprintType, Blueprintable)
class FORGERYUI_API UFUScreenProjectedWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, ScriptCallable, Category = "Projection")
	void SetProjectionSource(USceneComponent* InProjectionSource);

	UFUNCTION(BlueprintPure, ScriptCallable, Category = "Projection")
	USceneComponent* GetProjectionSource() const;

	UFUNCTION(BlueprintPure, ScriptCallable, Category = "Projection")
	float GetResolvedProjectionScale() const;

	UFUNCTION(BlueprintPure, ScriptCallable, Category = "Projection")
	bool IsProjectionClamped() const;

	UFUNCTION(BlueprintPure, ScriptCallable, Category = "Projection")
	EFUProjectedWidgetDistanceState GetDistanceState() const;

	void SetResolvedProjectionScale(float InResolvedProjectionScale);
	void SetProjectionClamped(bool bInProjectionClamped, bool bFireEvent);
	void SetDistanceState(EFUProjectedWidgetDistanceState InDistanceState, bool bFireEvent);
	void NotifyProjectionLayoutUpdated(const FFUProjectedWidgetLayoutContext& Context);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ScriptReadWrite, Category = "Projection")
	FFUProjectedWidgetSettings Settings;

protected:
	UPROPERTY(BlueprintReadOnly, ScriptReadOnly, Category = "Projection")
	TObjectPtr<USceneComponent> ProjectionSource;

	UPROPERTY(BlueprintReadOnly, ScriptReadOnly, Category = "Projection")
	float ResolvedProjectionScale = 1.0f;

	UPROPERTY(BlueprintReadOnly, ScriptReadOnly, Category = "Projection")
	bool bProjectionClamped = false;

	UPROPERTY(BlueprintReadOnly, ScriptReadOnly, Category = "Projection")
	EFUProjectedWidgetDistanceState DistanceState = EFUProjectedWidgetDistanceState::Normal;

	virtual void NativeOnProjectionLayoutUpdated(const FFUProjectedWidgetLayoutContext& Context);

	UFUNCTION(BlueprintImplementableEvent, Category = "Projection", meta = (DisplayName = "On Projection Source Set"))
	void ReceiveOnProjectionSourceSet(USceneComponent* InProjectionSource);

	UFUNCTION(BlueprintImplementableEvent, Category = "Projection", meta = (DisplayName = "On Clamped Changed"))
	void ReceiveOnClampedChanged(bool bIsClamped);

	UFUNCTION(BlueprintImplementableEvent, Category = "Projection", meta = (DisplayName = "On Distance State Changed"))
	void ReceiveOnDistanceStateChanged(EFUProjectedWidgetDistanceState NewDistanceState);

	UFUNCTION(BlueprintImplementableEvent, Category = "Projection", meta = (DisplayName = "On Projection Layout Updated"))
	void ReceiveOnProjectionLayoutUpdated(APlayerController* PlayerController, UWidget* ProjectionCanvas);
};
