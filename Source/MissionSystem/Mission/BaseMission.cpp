#include "BaseMission.h"
#include "Character/User/MissionSystemCharacter.h"

UBaseMission::UBaseMission()
{
}

void UBaseMission::Initialize()
{
	SetState(EMissionState::Ready);
}

void UBaseMission::SetState(EMissionState NewState)
{
	MissionState = NewState;

	switch (MissionState)
	{
		case EMissionState::Ready:      EnterReadyState(); break;
		case EMissionState::InProgress: EnterInProgressState();  break;
		case EMissionState::Succeeded:  EnterSucceededState();  break;
		case EMissionState::Failed:     EnterFailedState();  break;
	}

	OnMissionStateChanged.Broadcast(NewState);
}

void UBaseMission::EnterReadyState()
{
}

void UBaseMission::EnterInProgressState()
{
}

void UBaseMission::EnterSucceededState()
{
}

void UBaseMission::EnterFailedState()
{
}

void UBaseMission::AgreeMission(AMissionSystemCharacter* Player)
{
	MissionPlayer = Player;

	// 미션 진행 중으로 변경.
	SetState(EMissionState::InProgress);
}

AMissionSystemCharacter* UBaseMission::GetMissionPlayer() const
{
	return MissionPlayer.Get();
}

void UBaseMission::DisagreeMission()
{
}

void UBaseMission::SuccessMission()
{
	SetState(EMissionState::Succeeded);
}

void UBaseMission::FailedMission()
{
	SetState(EMissionState::Failed);
}
