class UFUButton : UFUButtonBase
{
	UPROPERTY(BindWidget)
	UCommonTextBlock Label;

	UPROPERTY(BindWidgetOptional)
	UCommonBorder Border;

	UPROPERTY(BindWidgetOptional)
	UPanelWidget SelectionElement;

	UPROPERTY(BindWidgetOptional)
	UCommonLazyImage Pic;

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

		if (Pic != nullptr)
		{
			Pic.SetBrushTintColor(FSlateColor(Label.ColorAndOpacity));
		}
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


	UFUNCTION(BlueprintOverride)
	void UpdateButtonStyle()
	{
		UpdatePicImage();
	}

	void UpdatePicImage()
	{
		if (Pic != nullptr)
		{
			int32 Index = !IsEnabled ? 2 : Selected ? 0 : 1;
			Pic.SetBrushFromSoftTexture(PicTextures[Index]);
		}		
	}
}