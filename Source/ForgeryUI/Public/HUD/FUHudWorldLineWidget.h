#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FUHudWorldLineWidget.generated.h"

class USceneComponent;
class UWidget;

/**
 * Draws a HUD-space line from a widget/local anchor to a projected world target.
 */
UCLASS(BlueprintType, Blueprintable)
class FORGERYUI_API UFUHudWorldLineWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, ScriptCallable, Category = "Projection|Line")
	void SetWorldTargetLocation(FVector WorldLocation);

	UFUNCTION(BlueprintCallable, ScriptCallable, Category = "Projection|Line")
	void SetWorldTargetComponent(USceneComponent* Component, FVector LocalOffset = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable, ScriptCallable, Category = "Projection|Line")
	void ClearWorldTarget();

	UFUNCTION(BlueprintCallable, ScriptCallable, Category = "Projection|Line")
	void SetStartLocalPosition(FVector2D LocalPosition);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Line", meta = (ClampMin = "0.0"))
	float LineThickness = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Line")
	FLinearColor Tint = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Line")
	FVector2D EndScreenOffset = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Line")
	bool bHideWhenProjectionFails = true;

protected:
	virtual void NativeConstruct() override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UWidget> StartAnchor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ScriptReadWrite, Category = "Projection|Line")
	FVector2D StartLocalPosition = FVector2D::ZeroVector;

private:
	bool ResolveWorldTarget(FVector& OutWorldLocation) const;
	bool ResolveStartPosition(const FGeometry& AllottedGeometry, FVector2D& OutStartPosition) const;
	bool ProjectWorldTarget(const FGeometry& AllottedGeometry, const FVector& WorldLocation, FVector2D& OutEndPosition) const;

	UPROPERTY(Transient)
	TWeakObjectPtr<USceneComponent> TargetComponent;

	FVector TargetLocalOffset = FVector::ZeroVector;
	FVector ExplicitWorldTargetLocation = FVector::ZeroVector;
	bool bHasExplicitWorldTarget = false;

	mutable TArray<FVector2D> PaintPointsScratch;
	mutable FVector2D LastProjectedEndPosition = FVector2D::ZeroVector;
	mutable bool bHasLastProjectedEndPosition = false;
};
