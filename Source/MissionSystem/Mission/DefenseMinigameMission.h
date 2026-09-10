#pragma once

#include "CoreMinimal.h"
#include "Mission/BaseMission.h"
#include "DefenseMinigameMission.generated.h"

// 쿼터뷰 디펜스 미니게임(몬스터 웨이브 생존)으로 진행되는 미션들의 공통 베이스.
// UDefenseMinigameController의 시작/종료를 State 훅(Enter*State)에 연결해,
// 미니게임의 소유권과 생명주기를 MissionManager가 아니라 미션 자신이 갖도록 함.
UCLASS(Abstract)
class MISSIONSYSTEM_API UDefenseMinigameMission : public UBaseMission
{
	GENERATED_BODY()

private:

	UPROPERTY()
	class UDefenseMinigameController* DefenseMinigame;

	UFUNCTION()
	void HandleMinigameSurvived();

protected:

	virtual void EnterInProgressState() override;

	virtual void EnterSucceededState() override;

	virtual void EnterFailedState() override;

public:

	virtual float GetMinigameRemainingTime() const override;

	virtual float GetMinigameTotalDuration() const override;
};
