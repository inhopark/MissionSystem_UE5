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

	// 스폰 중단 및 남은 몬스터 정리
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

private:

	TWeakObjectPtr<AActor> TargetPlayer;

	FTimerHandle SpawnTimerHandle;

	void SpawnMonster();
};
