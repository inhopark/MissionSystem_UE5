#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

class AMonster;

UCLASS()
class MISSIONSYSTEM_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()

public:
	AMonsterSpawner();

	// InTargetPlayer 앞쪽(위쪽)에서 몬스터를 주기적으로 스폰 시작
	void StartSpawning(AActor* InTargetPlayer);

	// 스폰 중단 및 남아있는 몬스터를 전부 풀로 반환
	void StopSpawning();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Spawn")
	TSubclassOf<AMonster> MonsterClass;

	// 스폰 간격(초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Spawn")
	float SpawnInterval = 0.5f;

	// 플레이어 기준 전방(X) 스폰 거리 - 디펜스 카메라 시야 위쪽에서 나타나도록 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Spawn")
	float SpawnDistanceX = 2500.0f;

	// 좌우(Y) 스폰 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Spawn")
	float SpawnRangeY = 1.0f;

	// 한 번의 스폰 타이밍에 동시에 등장할 몬스터 수 범위(난이도 조절용). [Min, Max] 사이에서 매번 랜덤
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Spawn")
	int32 MinSpawnCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Spawn")
	int32 MaxSpawnCount = 3;

private:

	TWeakObjectPtr<AActor> TargetPlayer;

	FTimerHandle SpawnTimerHandle;

	// 오브젝트 풀: 비활성 상태로 대기 중인 몬스터 / 현재 필드에 나와 있는 몬스터
	UPROPERTY()
	TArray<AMonster*> PooledMonsters;

	UPROPERTY()
	TArray<AMonster*> ActiveMonsters;

	// 타이머 콜백: MinSpawnCount~MaxSpawnCount 중 랜덤 개수만큼 SpawnOneMonster()를 호출
	void SpawnMonster();

	// 몬스터 한 마리를 풀에서 꺼내(또는 새로 생성해) 플레이어 앞쪽 랜덤 위치에 배치
	void SpawnOneMonster();

	// 풀에서 꺼내거나(있으면), 없으면 새로 SpawnActor해서 반환
	AMonster* AcquireMonster();

	UFUNCTION()
	void HandleMonsterDeactivated(AMonster* Monster);
};
