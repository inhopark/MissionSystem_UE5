#include "BaseMission.h"

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
		case EMissionState::Completed:  EnterCompletedState();  break;
	}

	OnMissionStateChanged.Broadcast(NewState);
}

void UBaseMission::EnterReadyState()
{
}

void UBaseMission::EnterInProgressState()
{
}

void UBaseMission::EnterCompletedState()
{
}

void UBaseMission::AgreeMission()
{
	// 미션 진행 중으로 변경.
	SetState(EMissionState::InProgress);
}

void UBaseMission::DisagreeMission()
{
}

void UBaseMission::SuccessMission()
{
	SetState(EMissionState::Completed);
}

void UBaseMission::FailedMission()
{
	SetState(EMissionState::Completed);
}