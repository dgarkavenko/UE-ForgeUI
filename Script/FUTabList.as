class UFUTabList : UCommonTabListWidgetBase
{
	UPROPERTY(BindWidget)
	UHorizontalBox Container;

	UPROPERTY()
	FMargin TabMargin;

	UPROPERTY()
	TMap<FName, FText> TabNames;

	UPROPERTY()
	TArray<FName> DisabledTabs;
	default DisabledTabs.Add(n"TabName2");

	UPROPERTY()
	TMap<FName, TSubclassOf<UCommonButtonStyle>> TabCustomStyles;

	default TabNames.Add(n"TabName0", FText());
	default TabNames.Add(n"TabName1", FText());
	default TabNames.Add(n"TabName2", FText());
	default TabNames.Add(n"TabName3", FText());

	UPROPERTY()
	TSubclassOf<UFUButton> ButtonClass;

	UFUNCTION(BlueprintOverride)
	void HandleTabCreation(FName TabNameID, UCommonButtonBase TabButton)
	{
		UHorizontalBoxSlot BoxSlot = Container.AddChildToHorizontalBox(TabButton);
		// FSlateChildSize Fill;
		// Fill.SizeRule = ESlateSizeRule::Fill;
		// Fill.Value = 1;
		
		// BoxSlot.SetSize(Fill);
		// BoxSlot.SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		// BoxSlot.SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		BoxSlot.SetPadding(TabMargin);		
		Cast<UFUButton>(TabButton).SetButtonText(TabNames[TabNameID]);
		
		TSubclassOf<UCommonButtonStyle> StyleClass;
		if (TabCustomStyles.Find(TabNameID, StyleClass))
		{
			TabButton.SetStyle(StyleClass);
		}
		
		if (DisabledTabs.Contains(TabNameID))
		{
			DisableTabWithReason(TabNameID, FText());
		}
	}

	UFUNCTION(BlueprintOverride)
	void HandleTabRemoval(FName TabNameID, UCommonButtonBase TabButton)
	{
		Container.RemoveChild(TabButton);
	}	
}