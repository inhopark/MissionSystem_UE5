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

	// DefenseMinigame이 있으면 그대로, 없으면 새로 생성해서 반환 (Start()는 호출하지 않음).
	// public인 이유: 자동 사이클 모드가 Agree 처리(EnterInProgressState -> Start()) 전에
	// 생존 시간을 미리 단축 설정할 수 있도록 컨트롤러 인스턴스에 먼저 접근해야 하기 때문
	class UDefenseMinigameController* GetOrCreateDefenseMinigameController();
};
