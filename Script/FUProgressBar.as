namespace ProgressBarMaterialParams
{
	const FName Value = n"Value";
	const FName ValueOffset = n"ValueOffset";
	const FName IsDamaged = n"bIsDamage";
	const FName BackgroundColor = n"BackgroundColor";
	const FName FillColor1 = n"FillColor1";
	const FName FillColor2 = n"FillColor2";
	const FName DecreaseColor = n"DecreaseColor";
	const FName IncreaseColor = n"IncreaseColor";
	const FName bUseBackgroundOpacityMask = n"bUseBackgroundOpacityMask";
	const FName BackgroundOpacityMask = n"BackgroundOpacityMask";
	const FName bUseFillColorOpacityMask = n"bUseFillColorOpacityMask";
	const FName FillColorOpacityMask = n"FillColorOpacityMask";
	const FName Smoothness = n"Smoothness";
	const FName Width = n"Width";
	const FName CircleLength = n"CircleLength";
	const FName ValueSmoothness = n"ValueSmoothness";
	const FName bInverseFillingDirection = n"bInverseFillingDirection";
	const FName SegmentNumber = n"SegmentNumber";
	const FName SegmentSize = n"SegmentSize";
	const FName SegmentSmoothnessSide1 = n"SegmentSmoothnessSide1";
	const FName SegmentSmoothnessSide2 = n"SegmentSmoothnessSide2";
}

class UFUProgressBar : UUserWidget
{
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar")
	UMaterialInterface BarMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Shape|Circular Properties")
	float Width = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Shape|Circular Properties")
	float CircleLength = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Shape|Segments")
	float SegmentNumber = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Shape|Segments")
	float SegmentSize = 0.8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Shape|Segments")
	bool bAlwaysFillSegments = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Shape|Segments")
	float StartSmoothness = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Shape|Segments")
	float EndSmoothness = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Shape|Segments")
	float SideSmoothness = 0.02f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Shape|Segments")
	float ValueSmoothness = 0.001f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Shape|Segments")
	bool bInverseFillingDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Color")
	FLinearColor FillColor1 = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Color")
	FLinearColor FillColor2 = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Color")
	FLinearColor BackgroundColor = FLinearColor(0, 0, 0, 0.2f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Color")
	UTexture2D FillColorOpacityMask;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Color")
	UTexture2D BackgroundOpacityMask;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Increase & Decrease Animations")
	FLinearColor IncreaseColor = FLinearColor(0.313f, 0.545f, 0.19f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Increase & Decrease Animations")
	FLinearColor DecreaseColor = FLinearColor(0.65f, 0.068f, 0.048f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Increase & Decrease Animations")
	float OffsetTime = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Increase & Decrease Animations")
	float OffsetSpeed = 0.5f;

	UPROPERTY(NotEditable, BlueprintReadOnly, Category = "Progress Bar|Runtime")
	UMaterialInstanceDynamic ImageMaterialDynamicInstance;

	UPROPERTY(NotEditable, BlueprintReadOnly, Category = "Progress Bar|Runtime")
	float TimeSinceLastValueUpdate = 1.0f;

	UPROPERTY(NotEditable, BlueprintReadOnly, Category = "Progress Bar|Runtime")
	bool bIsDamaged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Bar|Runtime")
	float Value = 1.0f;

	UPROPERTY(NotEditable, BlueprintReadOnly, Category = "Progress Bar|Runtime")
	private float CachedMaterialValue = 0.0f;

	UPROPERTY(NotEditable, BlueprintReadOnly, Category = "Progress Bar|Runtime")
	private float CachedMaterialValueOffset = 0.0f;

	private bool bPreviewActive;

	UPROPERTY(BindWidget, VisibleAnywhere, BlueprintReadOnly, Category = "Progress Bar|Runtime")
	UImage ImageBar;

	UFUNCTION(BlueprintOverride)
	void PreConstruct(bool IsDesignTime)
	{
		if (!TryInstantiateMaterial())
		{
			return;
		}

		UpdateStyle();
		SyncMaterialToValue();
	}

	UFUNCTION(BlueprintOverride)
	void Tick(FGeometry MyGeometry, float InDeltaTime)
	{
		if (bPreviewActive)
		{
			return;
		}

		if (Math::IsNearlyEqual(CachedMaterialValue, CachedMaterialValueOffset))
		{
			return;
		}

		TimeSinceLastValueUpdate += InDeltaTime;
		if (TimeSinceLastValueUpdate <= OffsetTime)
		{
			return;
		}

		float TargetValue = SegmentRound(Value);

		if (bIsDamaged)
		{
			float InterpValue = Math::FInterpConstantTo(CachedMaterialValueOffset, TargetValue, InDeltaTime, OffsetSpeed);
			ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::ValueOffset, InterpValue);
			CachedMaterialValueOffset = InterpValue;
		}
		else
		{
			float InterpValue = Math::FInterpConstantTo(CachedMaterialValue, TargetValue, InDeltaTime, OffsetSpeed);
			ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::Value, InterpValue);
			CachedMaterialValue = InterpValue;
		}
	}

	void SetValue(float InValue)
	{
		ApplyValue(InValue, false);
	}

	void SetValueAnimated(float InValue)
	{
		ApplyValue(InValue, true);
	}

	float GetValue()
	{
		return Value;
	}

	void ShowPreview(float InValue)
	{
		if (ImageMaterialDynamicInstance == nullptr)
		{
			return;
		}

		float PreviewValue = SegmentRound(Math::Clamp(InValue, 0.f, 1.f));
		float CurrentValue = SegmentRound(Value);
		float PreviewLower = Math::Min(PreviewValue, CurrentValue);
		float PreviewUpper = Math::Max(PreviewValue, CurrentValue);

		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::Value, PreviewLower);
		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::ValueOffset, PreviewUpper);
		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::IsDamaged, PreviewValue < CurrentValue ? 1 : 0);

		CachedMaterialValue = PreviewLower;
		CachedMaterialValueOffset = PreviewUpper;
		bPreviewActive = true;
	}

	void ClearPreview()
	{
		if (ImageMaterialDynamicInstance == nullptr)
		{
			return;
		}

		float SegmentRoundedValue = SegmentRound(Value);

		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::Value, SegmentRoundedValue);
		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::ValueOffset, SegmentRoundedValue);

		CachedMaterialValue = SegmentRoundedValue;
		CachedMaterialValueOffset = SegmentRoundedValue;
		bPreviewActive = false;
	}

	private void ApplyValue(float InValue, bool bAnimated)
	{
		if (ImageMaterialDynamicInstance == nullptr)
		{
			return;
		}

		float ClampedValue = Math::Clamp(InValue, 0.f, 1.f);
		if (Value == ClampedValue)
		{
			if (bPreviewActive)
			{
				ClearPreview();
			}

			return;
		}

		bIsDamaged = ClampedValue < Value;
		Value = ClampedValue;
		bPreviewActive = false;

		float SegmentRoundedValue = SegmentRound(Value);

		if (!bAnimated)
		{
			ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::Value, SegmentRoundedValue);
			ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::ValueOffset, SegmentRoundedValue);
			ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::IsDamaged, bIsDamaged ? 1 : 0);

			CachedMaterialValue = SegmentRoundedValue;
			CachedMaterialValueOffset = SegmentRoundedValue;
			return;
		}

		if (bIsDamaged)
		{
			ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::Value, SegmentRoundedValue);
			CachedMaterialValue = SegmentRoundedValue;
		}
		else
		{
			ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::ValueOffset, SegmentRoundedValue);
			CachedMaterialValueOffset = SegmentRoundedValue;
		}

		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::IsDamaged, bIsDamaged ? 1 : 0);
		TimeSinceLastValueUpdate = 0;
	}

	private bool TryInstantiateMaterial()
	{
		if (BarMaterial != nullptr)
		{
			ImageMaterialDynamicInstance = Material::CreateDynamicMaterialInstance(BarMaterial, n"BarMaterialInstance");
			ImageBar.SetBrushFromMaterial(ImageMaterialDynamicInstance);
			return true;
		}

		return false;
	}

	private void UpdateStyle()
	{
		ImageMaterialDynamicInstance.SetVectorParameterValue(ProgressBarMaterialParams::BackgroundColor, BackgroundColor);
		ImageMaterialDynamicInstance.SetVectorParameterValue(ProgressBarMaterialParams::FillColor1, FillColor1);
		ImageMaterialDynamicInstance.SetVectorParameterValue(ProgressBarMaterialParams::FillColor2, FillColor2);
		ImageMaterialDynamicInstance.SetVectorParameterValue(ProgressBarMaterialParams::DecreaseColor, DecreaseColor);
		ImageMaterialDynamicInstance.SetVectorParameterValue(ProgressBarMaterialParams::IncreaseColor, IncreaseColor);

		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::bUseBackgroundOpacityMask, IsValid(BackgroundOpacityMask) ? 1 : 0);
		ImageMaterialDynamicInstance.SetTextureParameterValue(ProgressBarMaterialParams::BackgroundOpacityMask, BackgroundOpacityMask);
		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::bUseFillColorOpacityMask, IsValid(FillColorOpacityMask) ? 1 : 0);
		ImageMaterialDynamicInstance.SetTextureParameterValue(ProgressBarMaterialParams::FillColorOpacityMask, FillColorOpacityMask);
		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::Smoothness, SideSmoothness);
		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::Width, Width);
		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::CircleLength, CircleLength);
		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::ValueSmoothness, ValueSmoothness);
		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::bInverseFillingDirection, bInverseFillingDirection ? 1 : 0);
		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::SegmentNumber, SegmentNumber);
		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::SegmentSize, SegmentSize);
		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::SegmentSmoothnessSide1, StartSmoothness);
		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::SegmentSmoothnessSide2, EndSmoothness);
	}

	private void SyncMaterialToValue()
	{
		float SegmentRoundedValue = SegmentRound(Value);

		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::Value, SegmentRoundedValue);
		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::ValueOffset, SegmentRoundedValue);
		ImageMaterialDynamicInstance.SetScalarParameterValue(ProgressBarMaterialParams::IsDamaged, 0.0f);

		CachedMaterialValue = SegmentRoundedValue;
		CachedMaterialValueOffset = SegmentRoundedValue;
		bPreviewActive = false;
	}

	private float SegmentRound(float In)
	{
		return bAlwaysFillSegments ? Math::RoundToInt(In * SegmentNumber) / SegmentNumber : In;
	}
}
