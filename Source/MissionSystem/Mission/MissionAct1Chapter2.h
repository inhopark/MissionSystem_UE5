#pragma once

#include "CoreMinimal.h"
#include "Mission/DefenseMinigameMission.h"
#include "MissionAct1Chapter2.generated.h"

// 디펜스 미니게임 시작/종료는 UDefenseMinigameMission이 처리함.
// 여기서는 미션별 State 훅(로그, 연출, 보상 등)만 필요에 따라 추가하면 됨.
UCLASS()
class MISSIONSYSTEM_API UMissionAct1Chapter2 : public UDefenseMinigameMission
{
	GENERATED_BODY()

protected:

	virtual void EnterReadyState() override;
	virtual void EnterInProgressState() override;
	virtual void EnterSucceededState() override;
	virtual void EnterFailedState() override;
};
