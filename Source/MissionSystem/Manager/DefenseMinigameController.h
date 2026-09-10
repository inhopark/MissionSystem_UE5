#pragma once

#include "CoreMinimal.h"
#include "DefenseMinigameController.generated.h"

class AMonsterSpawner;
class AMissionSystemCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDefenseMinigameSurvived);

// 디펜스 미니게임(몬스터 스폰 + 생존 타이머)의 시작/종료만을 전담하는 컨트롤러.
// UMissionManager가 미션 상태 전이에만 집중할 수 있도록 몬스터/타이머 관련 책임을 분리함(SRP).
UCLASS()
class MISSIONSYSTEM_API UDefenseMinigameController : public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY()
	AMonsterSpawner* MonsterSpawner;

	// 이 시간(초) 동안 생존하면 OnSurvived 브로드캐스트
	UPROPERTY(EditAnywhere, Category = "Defense")
	float SurvivalDuration = 30.0f;

	FTimerHandle SurvivalTimerHandle;

	AMonsterSpawner* GetOrCreateMonsterSpawner();

	UFUNCTION()
	void HandleTimerExpired();

public:

	UPROPERTY()
	FOnDefenseMinigameSurvived OnSurvived;

	// 몬스터 스폰과 생존 타이머를 시작
	void Start(AMissionSystemCharacter* Player);

	// 몬스터 스폰과 생존 타이머를 중단 (성공/실패/조기 종료 등 모든 경우에 호출)
	void Stop();

	// 남은 생존 시간(초). 진행 중이 아니면 음수 반환
	float GetRemainingTime() const;

	FORCEINLINE float GetTotalDuration() const { return SurvivalDuration; }
};
