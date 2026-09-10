#include "MainMissionWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UMainMissionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (LeftButton)
	{
		LeftButton->OnClicked.AddDynamic(this, &UMainMissionWidget::OnLeftButtonClicked);
	}

	if (RightButton)
	{
		RightButton->OnClicked.AddDynamic(this, &UMainMissionWidget::OnRightButtonClicked);
	}
}

void UMainMissionWidget::SetWidgetState(EMissionState MissionState)
{
	switch (MissionState)
	{
		case EMissionState::Ready:
			SetMissionTitle(WAIT_FOR_MISSION);
			SetButtonTexts(MISSION_AGREE_BUTTON_TEXT, MISSION_DISAGREE_BUTTON_TEXT);
			break;
		case EMissionState::InProgress:
			SetMissionTitle(MISSION_IN_PROGRESS);
			SetButtonTexts(MISSION_SUCCESS_BUTTON_TEXT, MISSION_FAILED_BUTTON_TEXT);
			break;
		default:
			// 미션 성공/실패 시에는 결과 전용 위젯(WBP_MainMissionResult)이 대신 표시되며,
			// 이 대화창 자체는 MissionManager에 의해 숨겨지므로 별도 처리가 필요 없음.
			break;
	}

	// 버튼 보여질지 선택.
	bool bButtonVisible = MissionState == EMissionState::Ready || MissionState == EMissionState::InProgress;
	if (LeftButton)
	{
		LeftButton->SetVisibility(bButtonVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	if (RightButton)
	{
		RightButton->SetVisibility(bButtonVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UMainMissionWidget::SetMissionTitle(const FText& NewTitle)
{
	if (MissionTitleText)
	{
		MissionTitleText->SetText(NewTitle);
	}
}

void UMainMissionWidget::SetButtonTexts(const FText& LeftText, const FText& RightText)
{
	if (LeftButtonTextBlock)
	{
		LeftButtonTextBlock->SetVisibility(ESlateVisibility::Visible);
		LeftButtonTextBlock->SetText(LeftText);
	}
	if (RightButtonTextBlock)
	{
		RightButtonTextBlock->SetVisibility(ESlateVisibility::Visible);
		RightButtonTextBlock->SetText(RightText);
	}
}

void UMainMissionWidget::OnLeftButtonClicked()
{
	OnMissionButtonAction.Broadcast(EMissionButtonAction::Left);
}

void UMainMissionWidget::OnRightButtonClicked()
{
	OnMissionButtonAction.Broadcast(EMissionButtonAction::Right);
}
