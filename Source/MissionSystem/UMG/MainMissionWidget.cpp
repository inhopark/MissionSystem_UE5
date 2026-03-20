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
            {
               SetMissionTitle(WAIT_FOR_MISSION);
               SetButtonTexts(MISSION_AGREE_BUTTON_TEXT, MISSION_DISAGREE_BUTTON_TEXT);
            }
            break;
        case EMissionState::InProgress:
            {
                SetMissionTitle(MISSION_IN_PROGRESS);
                SetButtonTexts(MISSION_SUCCESS_BUTTON_TEXT, MISSION_FAILED_BUTTON_TEXT);
            }
            break;
        case EMissionState::Completed:
            {
                SetMissionTitle(MISSION_COMPLETED);
            }
            break;
    }

    // 버튼 보여질지 선택.
    bool bButtonVisible = MissionState != EMissionState::Completed;
    if (LeftButton)
    {
        LeftButton->SetVisibility(bButtonVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
    if (RightButton)
    {
        RightButton->SetVisibility(bButtonVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void UMainMissionWidget::SetMissionTitle(const FString& NewTitle)
{
    if (MissionTitleText)
    {
        MissionTitleText->SetText(FText::FromString(NewTitle));
    }
}

void UMainMissionWidget::SetButtonTexts(const FString& LeftText, const FString& RightText)
{
    if (LeftButtonTextBlock)
    {
		LeftButtonTextBlock->SetVisibility(ESlateVisibility::Visible);
        LeftButtonTextBlock->SetText(FText::FromString(LeftText));
    }
    if (RightButtonTextBlock)
    {
        RightButtonTextBlock->SetVisibility(ESlateVisibility::Visible);
        RightButtonTextBlock->SetText(FText::FromString(RightText));
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