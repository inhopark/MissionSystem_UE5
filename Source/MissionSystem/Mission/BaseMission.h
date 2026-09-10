#pragma once

#include "CoreMinimal.h"
#include "Define/MissionDefine.h"
#include "BaseMission.generated.h"

class AMissionSystemCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionStateChanged, EMissionState, NewState);

UCLASS()
class MISSIONSYSTEM_API UBaseMission : public UObject
{
	GENERATED_BODY()

private:

	EMissionState MissionState;

public:

	UPROPERTY()
	FOnMissionStateChanged OnMissionStateChanged;

private:

	void SetState(EMissionState NewState);

protected:

	// AgreeMission()에서 저장됨. EnterInProgressState() 등 State 훅에서 사용 가능
	TWeakObjectPtr<AMissionSystemCharacter> MissionPlayer;

	AMissionSystemCharacter* GetMissionPlayer() const;

	virtual void EnterReadyState();

	virtual void EnterInProgressState();

	virtual void EnterSucceededState();

	virtual void EnterFailedState();

public:

	FORCEINLINE EMissionState GetMissionState() const { return MissionState; }

public:

	virtual void Initialize();

	virtual void AgreeMission(AMissionSystemCharacter* Player);

	virtual void DisagreeMission();

	virtual void SuccessMission();

	virtual void FailedMission();

	// 이 미션이 디펜스 미니게임을 진행 중이라면 남은 시간(초)을 반환. 없으면 음수(기본 구현)
	virtual float GetMinigameRemainingTime() const { return -1.0f; }

	// 이 미션이 디펜스 미니게임을 진행 중이라면 총 제한시간(초)을 반환. 없으면 0(기본 구현)
	virtual float GetMinigameTotalDuration() const { return 0.0f; }

public:

	UBaseMission();

};
