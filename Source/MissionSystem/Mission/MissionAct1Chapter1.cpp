#include "MissionAct1Chapter1.h"

void UMissionAct1Chapter1::EnterReadyState()
{
	Super::EnterReadyState();

	UE_LOG(LogTemp, Log, TEXT("## MissionAct1Chapter1 ## 준비 상태 진입"));
}

void UMissionAct1Chapter1::EnterInProgressState()
{
	Super::EnterInProgressState();

	UE_LOG(LogTemp, Log, TEXT("## MissionAct1Chapter1 ## 진행 상태 진입"));
}

void UMissionAct1Chapter1::EnterSucceededState()
{
	Super::EnterSucceededState();

	UE_LOG(LogTemp, Log, TEXT("## MissionAct1Chapter1 ## 성공 상태 진입"));
}

void UMissionAct1Chapter1::EnterFailedState()
{
	Super::EnterFailedState();

	UE_LOG(LogTemp, Log, TEXT("## MissionAct1Chapter1 ## 실패 상태 진입"));
}
