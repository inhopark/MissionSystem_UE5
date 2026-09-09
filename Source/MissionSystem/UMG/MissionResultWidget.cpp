#include "MissionResultWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Define/MissionDefine.h"

void UMissionResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (LeftButton)
	{
		LeftButton->OnClicked.AddDynamic(this, &UMissionResultWidget::OnConfirmButtonClicked);
	}

	if (LeftButtonTextBlock)
	{
		LeftButtonTextBlock->SetText(MISSION_CONFIRM_BUTTON_TEXT);
	}
}

void UMissionResultWidget::SetResultTitle(const FText& NewTitle)
{
	if (MissionTitleText)
	{
		MissionTitleText->SetText(NewTitle);
	}
}

void UMissionResultWidget::OnConfirmButtonClicked()
{
	OnConfirmed.Broadcast();
}

void UMissionResultWidget::SetResultState(EMissionState NewState)
{
	ResultState = NewState;

	if(ResultState == EMissionState::Succeeded)
	{
		SetResultTitle(MISSION_SUCCESS);
	}
	else if(ResultState == EMissionState::Failed)
	{
		SetResultTitle(MISSION_FAILED);
	}
}
