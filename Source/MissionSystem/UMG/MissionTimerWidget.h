#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MissionTimerWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class MISSIONSYSTEM_API UMissionTimerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* TimerBar;   // 남은 시간 게이지 (선택, 없어도 동작)

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TimerText;    // 남은 초 텍스트

public:

	// RemainingTime/TotalDuration(초)로 게이지, 텍스트 갱신
	void SetRemainingTime(float RemainingTime, float TotalDuration);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
