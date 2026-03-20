#pragma once

#include "CoreMinimal.h"
#include "Mission/BaseMission.h"
#include "MissionAct1Chapter2.generated.h"

UCLASS()
class MISSIONSYSTEM_API UMissionAct1Chapter2 : public UBaseMission
{
	GENERATED_BODY()

protected:

	virtual void EnterReadyState() override;

	virtual void EnterInProgressState() override;

	virtual void EnterCompletedState() override;

public:

	virtual void Initialize() override;

	virtual void AgreeMission() override;

	virtual void DisagreeMission() override;

	virtual void SuccessMission() override;

	virtual void FailedMission() override;

};
