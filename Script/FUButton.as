class UFUButton : UFUButtonBase
{
	UPROPERTY(BindWidget)
	UCommonTextBlock Label;

	UPROPERTY(BindWidgetOptional)
	UCommonBorder Border;

	UPROPERTY(BindWidgetOptional)
	UCommonBorder BorderBorder;

	UPROPERTY(BindWidgetOptional)
	UPanelWidget SelectionElement;

	UPROPERTY(BindWidgetOptional)
	UCommonLazyImage Pic;

	UPROPERTY(BindWidgetOptional)
	UProgressBar HoldFill;

	UPROPERTY()
	TArray<TSoftObjectPtr<UTexture2D>> PicTextures;
	
	UFUNCTION(BlueprintOverride)
	void UpdateButtonText(FText InText)
	{
		Label.SetText(InText);		
	}

	UFUNCTION(BlueprintOverride)
	void OnCurrentTextStyleChanged()
	{
		Label.SetStyle(CurrentTextStyleClass);
		if (Border != nullptr)
		{
			FSlateBrush Brush;
			Brush.TintColor = Label.ColorAndOpacity;
			
			Brush.TintColor.ColorUseRule = ESlateColorStylingMode::UseColor_Specified;
			Brush.TintColor.SpecifiedColor.A = 1.0;
			Border.SetBrush(Brush);
		}

		if (BorderBorder != nullptr)
		{
			FLinearColor Color = Label.ColorAndOpacity.SpecifiedColor;
			Color.A = bIsEnabled ? Label.ColorAndOpacity.SpecifiedColor.A : 0;
			BorderBorder.SetBrushColor(Color);
		}

		if (Pic != nullptr)
		{
			Pic.SetBrushTintColor(FSlateColor(Label.ColorAndOpacity));
		}

		UpdatePicImage();
	}

	UFUNCTION(BlueprintOverride)
	void OnSelected()
	{
		if (Border != nullptr)
		{			
			Border.SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		if (SelectionElement != nullptr)
		{
			SelectionElement.SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		UpdatePicImage();
	}

	UFUNCTION(BlueprintOverride)
	void OnDeselected()
	{
		if (Border != nullptr)
		{			
			Border.SetVisibility(ESlateVisibility::Hidden);
		}

		if (SelectionElement != nullptr)
		{
			SelectionElement.SetVisibility(ESlateVisibility::Hidden);
		}

		UpdatePicImage();
	}

	// Normalized 0..1 from CommonUI's hold ticker; also driven back down by hold rollback.
	UFUNCTION(BlueprintOverride)
	void OnActionProgress(float HeldPercent)
	{
		if (HoldFill != nullptr)
		{
			HoldFill.SetPercent(HeldPercent);
		}
	}

	void UpdatePicImage()
	{
		if (Pic != nullptr)
		{
			int32 Index = (!IsEnabled || Locked) ? 2 : Selected ? 0 : 1;
			Pic.SetBrushFromSoftTexture(PicTextures[Index]);			
		}		
	}
}