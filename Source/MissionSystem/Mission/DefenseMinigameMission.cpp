#include "DefenseMinigameMission.h"
#include "Manager/DefenseMinigameController.h"
#include "Character/User/MissionSystemCharacter.h"

void UDefenseMinigameMission::EnterInProgressState()
{
	Super::EnterInProgressState();

	if (DefenseMinigame == nullptr)
	{
		DefenseMinigame = NewObject<UDefenseMinigameController>(this);
		DefenseMinigame->OnSurvived.AddDynamic(this, &UDefenseMinigameMission::HandleMinigameSurvived);
	}

	DefenseMinigame->Start(GetMissionPlayer());
}

void UDefenseMinigameMission::EnterSucceededState()
{
	Super::EnterSucceededState();

	if (DefenseMinigame != nullptr)
	{
		DefenseMinigame->Stop();
	}
}

void UDefenseMinigameMission::EnterFailedState()
{
	Super::EnterFailedState();

	if (DefenseMinigame != nullptr)
	{
		DefenseMinigame->Stop();
	}
}

void UDefenseMinigameMission::HandleMinigameSurvived()
{
	SuccessMission();
}

float UDefenseMinigameMission::GetMinigameRemainingTime() const
{
	return DefenseMinigame != nullptr ? DefenseMinigame->GetRemainingTime() : -1.0f;
}

float UDefenseMinigameMission::GetMinigameTotalDuration() const
{
	return DefenseMinigame != nullptr ? DefenseMinigame->GetTotalDuration() : 0.0f;
}
