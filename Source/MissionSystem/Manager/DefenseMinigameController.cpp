#include "DefenseMinigameController.h"
#include "Spawner/MonsterSpawner.h"
#include "Character/User/MissionSystemCharacter.h"

void UDefenseMinigameController::Start(AMissionSystemCharacter* Player)
{
	if (Player == nullptr)
	{
		return;
	}

	if (AMonsterSpawner* Spawner = GetOrCreateMonsterSpawner())
	{
		Spawner->StartSpawning(Player);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(SurvivalTimerHandle, this, &UDefenseMinigameController::HandleTimerExpired, SurvivalDuration, false);
	}
}

void UDefenseMinigameController::Stop()
{
	if (MonsterSpawner != nullptr)
	{
		MonsterSpawner->StopSpawning();
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SurvivalTimerHandle);
	}
}

void UDefenseMinigameController::HandleTimerExpired()
{
	OnSurvived.Broadcast();
}

float UDefenseMinigameController::GetRemainingTime() const
{
	if (const UWorld* World = GetWorld())
	{
		return World->GetTimerManager().GetTimerRemaining(SurvivalTimerHandle);
	}

	return -1.0f;
}

AMonsterSpawner* UDefenseMinigameController::GetOrCreateMonsterSpawner()
{
	if (MonsterSpawner == nullptr)
	{
		if (UWorld* World = GetWorld())
		{
			MonsterSpawner = World->SpawnActor<AMonsterSpawner>();
		}
	}

	return MonsterSpawner;
}
