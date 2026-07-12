#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUD/FUProjectedWidget_Frame.h"
#include "HUD/FUScreenProjectedWidget.h"
#include "FUProjectedWidgetHost.generated.h"

class UCanvasPanel;
class USceneComponent;

/**
 * Shared screen-space projection host for world markers embedded in a HUD widget.
 */
UCLASS(BlueprintType, Blueprintable)
class FORGERYUI_API UFUProjectedWidgetHost : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, ScriptCallable, Category = "Projection", Meta = (DeterminesOutputType = "WidgetClass"))
	UFUScreenProjectedWidget* RegisterWidget(
		USceneComponent* Anchor,
		TSubclassOf<UFUScreenProjectedWidget> WidgetClass,
		FVector AnchorLocalOffset = FVector::ZeroVector,
		FVector2D ScreenOffset = FVector2D::ZeroVector,
		int32 ZOrder = 0);

	UFUNCTION(BlueprintCallable, ScriptCallable, Category = "Projection")
	void UnregisterWidget(USceneComponent* Anchor);

	UFUNCTION(BlueprintCallable, ScriptCallable, Category = "Projection")
	void UnregisterWidgetInstance(UFUScreenProjectedWidget* Widget);

	UFUNCTION(BlueprintCallable, ScriptCallable, Category = "Projection", Meta = (DeterminesOutputType = "WidgetClass"))
	UFUProjectedWidget_Frame* RegisterFrameWidget(
		USceneComponent* Anchor,
		TSubclassOf<UFUProjectedWidget_Frame> WidgetClass,
		FVector AnchorLocalOffset = FVector::ZeroVector,
		FVector2D ScreenOffset = FVector2D::ZeroVector,
		int32 ZOrder = 0);

	UFUNCTION(BlueprintCallable, ScriptCallable, Category = "Projection")
	void UpdateFrameWidget(
		UFUScreenProjectedWidget* Widget,
		FVector AnchorLocalOffset = FVector::ZeroVector);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> MarkerCanvas;

private:
	void RemoveEntryAt(int32 Index);
	void ApplySettingsAt(int32 Index, const FFUProjectedWidgetSettings& Settings);

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<USceneComponent>> Anchors;

	UPROPERTY(Transient)
	TArray<FVector> AnchorPositions;

	UPROPERTY(Transient)
	TArray<FVector> AnchorLocalOffsets;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UFUScreenProjectedWidget>> Widgets;

	UPROPERTY(Transient)
	TArray<TSubclassOf<UFUScreenProjectedWidget>> WidgetClasses;

	UPROPERTY(Transient)
	TArray<FVector2D> ScreenOffsets;

	UPROPERTY(Transient)
	TArray<FVector2D> Pivots;

	UPROPERTY(Transient)
	TArray<int32> ZOrders;

	UPROPERTY(Transient)
	TArray<EFUProjectedWidgetOffscreenMode> OffscreenModes;

	UPROPERTY(Transient)
	TArray<float> ClampMargins;

	UPROPERTY(Transient)
	TArray<float> MinVisibleDistances;

	UPROPERTY(Transient)
	TArray<float> NearDistances;

	UPROPERTY(Transient)
	TArray<float> FarDistances;

	UPROPERTY(Transient)
	TArray<float> MaxVisibleDistances;

	UPROPERTY(Transient)
	TArray<EFUProjectedWidgetScaleMode> ScaleModes;

	UPROPERTY(Transient)
	TArray<float> ConstantScales;

	UPROPERTY(Transient)
	TArray<float> ReferenceDistances;

	UPROPERTY(Transient)
	TArray<float> ReferenceScales;

	UPROPERTY(Transient)
	TArray<float> MinScales;

	UPROPERTY(Transient)
	TArray<float> MaxScales;

	UPROPERTY(Transient)
	TArray<float> NearScales;

	UPROPERTY(Transient)
	TArray<float> NormalScales;

	UPROPERTY(Transient)
	TArray<float> FarScales;

	UPROPERTY(Transient)
	TArray<EFUProjectedWidgetDistanceState> DistanceStates;

	UPROPERTY(Transient)
	TArray<float> ResolvedScales;

	UPROPERTY(Transient)
	TArray<uint8> ClampedStates;

	UPROPERTY(Transient)
	TArray<uint8> InitializedStates;
};
