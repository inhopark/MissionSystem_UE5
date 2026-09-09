#include "MonsterSpawner.h"
#include "Character/Monster/Monster.h"
#include "Kismet/GameplayStatics.h"

AMonsterSpawner::AMonsterSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	MonsterClass = AMonster::StaticClass();
}

void AMonsterSpawner::StartSpawning(AActor* InTargetPlayer)
{
	TargetPlayer = InTargetPlayer;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SpawnTimerHandle);
		World->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AMonsterSpawner::SpawnMonster, SpawnInterval, true, 0.5f);
	}
}

void AMonsterSpawner::StopSpawning()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SpawnTimerHandle);
	}

	// 미션 종료 시 화면에 남아있는 몬스터 정리
	TArray<AActor*> RemainingMonsters;
	UGameplayStatics::GetAllActorsOfClass(this, AMonster::StaticClass(), RemainingMonsters);
	for (AActor* Monster : RemainingMonsters)
	{
		if (Monster != nullptr)
		{
			Monster->Destroy();
		}
	}

	TargetPlayer = nullptr;
}

void AMonsterSpawner::SpawnMonster()
{
	if (!TargetPlayer.IsValid() || MonsterClass == nullptr)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const FVector PlayerLocation = TargetPlayer->GetActorLocation();
	const float RandomY = FMath::FRandRange(-SpawnRangeY, SpawnRangeY);
	const FVector SpawnLocation = PlayerLocation + FVector(SpawnDistanceX, RandomY, 0.0f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	World->SpawnActor<AMonster>(MonsterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
}
