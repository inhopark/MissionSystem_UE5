#include "MissionAct1Chapter2.h"

void UMissionAct1Chapter2::EnterReadyState()
{
	Super::EnterReadyState();

	UE_LOG(LogTemp, Log, TEXT("## MissionAct1Chapter2 ## 준비 상태 진입"));
}

void UMissionAct1Chapter2::EnterInProgressState()
{
	Super::EnterInProgressState();

	UE_LOG(LogTemp, Log, TEXT("## MissionAct1Chapter2 ## 진행 상태 진입"));
}

void UMissionAct1Chapter2::EnterSucceededState()
{
	Super::EnterSucceededState();

	UE_LOG(LogTemp, Log, TEXT("## MissionAct1Chapter2 ## 성공 상태 진입"));
}

void UMissionAct1Chapter2::EnterFailedState()
{
	Super::EnterFailedState();

	UE_LOG(LogTemp, Log, TEXT("## MissionAct1Chapter2 ## 실패 상태 진입"));
}
