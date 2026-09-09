#include "MissionTimerWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Manager/MissionManager.h"
#include "Kismet/GameplayStatics.h"

void UMissionTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UMissionManager* pMissionManager = GameInstance->GetSubsystem<UMissionManager>())
		{
			const float Remaining = pMissionManager->GetMissionRemainingTime();
			if (Remaining >= 0.0f)
			{
				SetRemainingTime(Remaining, pMissionManager->GetMissionTotalDuration());
			}
		}
	}
}

void UMissionTimerWidget::SetRemainingTime(float RemainingTime, float TotalDuration)
{
	if (TimerBar != nullptr)
	{
		TimerBar->SetPercent(TotalDuration > 0.0f ? RemainingTime / TotalDuration : 0.0f);
	}

	if (TimerText != nullptr)
	{
		TimerText->SetText(FText::AsNumber(FMath::CeilToInt(RemainingTime)));
	}
}
