#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "ForgeTabListWidgetBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FForgeOnTabContentCreated, FName, TabId, UCommonUserWidget*, TabWidget);
DECLARE_EVENT_TwoParams(UForgeTabListWidgetBase, FForgeOnTabContentCreatedNative, FName /* TabId */, UCommonUserWidget* /* TabWidget */);

USTRUCT(BlueprintType)
struct FForgeTabDescriptor
{
	GENERATED_BODY()

public:
	FForgeTabDescriptor()
		: bHidden(false)
		, CreatedTabContentWidget(nullptr)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName TabId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TabText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush IconBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bHidden;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCommonButtonBase> TabButtonType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCommonUserWidget> TabContentType;

	UPROPERTY(Transient)
	TObjectPtr<UWidget> CreatedTabContentWidget;
};

UINTERFACE(BlueprintType)
class UForgeTabButtonInterface : public UInterface
{
	GENERATED_BODY()
};

class IForgeTabButtonInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Tab Button")
	void SetTabLabelInfo(const FForgeTabDescriptor& TabDescriptor);
};

/**
 * Tab list widget with tab descriptors, dynamic registration, and visibility management.
 */
UCLASS()
class FORGEUI_API UForgeTabListWidgetBase : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab List")
	bool GetPreregisteredTabInfo(const FName TabNameId, FForgeTabDescriptor& OutTabInfo);

	const TArray<FForgeTabDescriptor>& GetAllPreregisteredTabInfos() { return PreregisteredTabInfoArray; }

	UFUNCTION(BlueprintCallable, Category = "Tab List")
	void SetTabHiddenState(FName TabNameId, bool bHidden);

	UFUNCTION(BlueprintCallable, Category = "Tab List")
	bool RegisterDynamicTab(const FForgeTabDescriptor& TabDescriptor);

	UFUNCTION(BlueprintCallable, Category = "Tab List")
	bool IsFirstTabActive() const;

	UFUNCTION(BlueprintCallable, Category = "Tab List")
	bool IsLastTabActive() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab List")
	bool IsTabVisible(FName TabId) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab List")
	int32 GetVisibleTabCount();

	UPROPERTY(BlueprintAssignable, Category = "Tab List")
	FForgeOnTabContentCreated OnTabContentCreated;
	FForgeOnTabContentCreatedNative OnTabContentCreatedNative;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void HandlePreLinkedSwitcherChanged() override;
	virtual void HandlePostLinkedSwitcherChanged() override;

	virtual void HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton) override;

private:
	void SetupTabs();

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "TabId"))
	TArray<FForgeTabDescriptor> PreregisteredTabInfoArray;

	UPROPERTY()
	TMap<FName, FForgeTabDescriptor> PendingTabLabelInfoMap;
};
