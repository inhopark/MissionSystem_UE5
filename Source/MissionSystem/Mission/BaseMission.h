#pragma once

#include "CoreMinimal.h"
#include "Define/MissionDefine.h"
#include "BaseMission.generated.h"

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

	virtual void EnterReadyState();

	virtual void EnterInProgressState();

	virtual void EnterCompletedState();

public:

	EMissionState GetMissionState() { return MissionState; }

public:

	virtual void Initialize();

	virtual void AgreeMission();

	virtual void DisagreeMission();

	virtual void SuccessMission();

	virtual void FailedMission();

public:

	UBaseMission();

};
