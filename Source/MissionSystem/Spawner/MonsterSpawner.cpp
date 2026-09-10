#include "MonsterSpawner.h"
#include "Character/Monster/Monster.h"

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

	// 미션 종료 시 화면에 남아있는 몬스터를 전부 풀로 반환.
	// Deactivate()가 HandleMonsterDeactivated를 통해 ActiveMonsters를 수정하므로 복사본을 순회함
	TArray<AMonster*> MonstersToDeactivate = ActiveMonsters;
	for (AMonster* Monster : MonstersToDeactivate)
	{
		if (Monster != nullptr)
		{
			Monster->Deactivate();
		}
	}

	TargetPlayer = nullptr;
}

void AMonsterSpawner::SpawnMonster()
{
	if (TargetPlayer.IsValid() == false || MonsterClass == nullptr)
	{
		return;
	}

	// 난이도를 위해 한 번에 여러 마리를 랜덤으로 동시에 등장시킴
	const int32 SpawnCount = FMath::RandRange(MinSpawnCount, MaxSpawnCount);
	for (int32 Index = 0; Index < SpawnCount; ++Index)
	{
		SpawnOneMonster();
	}
}

void AMonsterSpawner::SpawnOneMonster()
{
	AMonster* Monster = AcquireMonster();
	if (Monster == nullptr)
	{
		return;
	}

	const FVector PlayerLocation = TargetPlayer->GetActorLocation();
	const float RandomY = FMath::FRandRange(-SpawnRangeY, SpawnRangeY);
	const FVector SpawnLocation = PlayerLocation + FVector(SpawnDistanceX, RandomY, 0.0f);

	Monster->ActivateAt(SpawnLocation);
	ActiveMonsters.Add(Monster);
}

AMonster* AMonsterSpawner::AcquireMonster()
{
	// 오브젝트 풀 패턴: 대기 중인 몬스터가 있으면 재사용, 없을 때만 새로 생성해
	// 디펜스 웨이브 내내 반복되는 Spawn/Destroy 비용을 없앰
	if (PooledMonsters.Num() > 0)
	{
		return PooledMonsters.Pop();
	}

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AMonster* NewMonster = World->SpawnActor<AMonster>(MonsterClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (NewMonster != nullptr)
	{
		NewMonster->OnDeactivated.AddDynamic(this, &AMonsterSpawner::HandleMonsterDeactivated);
	}

	return NewMonster;
}

void AMonsterSpawner::HandleMonsterDeactivated(AMonster* Monster)
{
	if (Monster == nullptr)
	{
		return;
	}

	ActiveMonsters.RemoveSingleSwap(Monster);
	PooledMonsters.Add(Monster);
}
