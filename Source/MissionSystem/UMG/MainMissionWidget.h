#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Define/MissionDefine.h"
#include "MainMissionWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionButtonAction, EMissionButtonAction, Action);

UCLASS()
class MISSIONSYSTEM_API UMainMissionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MissionTitleText;  // 미션 제목 텍스트

	UPROPERTY(meta = (BindWidget))
	class UButton* LeftButton;           // 왼쪽 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* RightButton;          // 오른쪽 버튼

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LeftButtonTextBlock;   // 왼쪽 버튼 텍스트
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RightButtonTextBlock;  // 오른쪽 버튼 텍스트

public:

	UPROPERTY()
	FOnMissionButtonAction OnMissionButtonAction;

public:

	void SetMissionTitle(const FString& NewTitle);  // 미션 제목 설정
	void SetButtonTexts(const FString& LeftText, const FString& RightText);  // 버튼 텍스트 설정
	void SetWidgetState(EMissionState MissionState);

	UFUNCTION()
	void OnLeftButtonClicked();

	UFUNCTION()
	void OnRightButtonClicked();

public:

	virtual void NativeConstruct() override;

};
