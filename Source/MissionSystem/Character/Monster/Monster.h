#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Monster.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class AMonster;

// 몬스터가 비활성화(오브젝트 풀로 반환)될 때 스포너에 알리는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterDeactivated, AMonster*, Monster);

UCLASS()
class MISSIONSYSTEM_API AMonster : public AActor
{
	GENERATED_BODY()

public:
	AMonster();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	UStaticMeshComponent* MonsterMesh;

	// 초당 이동 속도 (월드 -X 방향, 화면상으로는 위에서 아래로 내려오는 방향)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float MoveSpeed = 900.0f;

	// 이 거리만큼 이동하면(플레이어를 지나쳤다고 판단) 풀로 반환
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float MaxTravelDistance = 3000.0f;

	// 플레이어와 충돌 시 깎을 HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float DamageAmount = 10.0f;

private:

	float DistanceTraveled = 0.0f;

	// 이미 비활성화된 몬스터가 같은 프레임에 두 번 Deactivate되어
	// 오브젝트 풀에 중복으로 반환되는 것을 막기 위한 가드
	bool bIsActive = false;

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:

	// 오브젝트 풀에서 꺼내 재사용할 때 호출 — 위치 리셋, 표시/충돌/틱 활성화
	void ActivateAt(const FVector& SpawnLocation);

	// 오브젝트 풀로 반환할 때 호출 — 숨김, 충돌/틱 비활성화, OnDeactivated 브로드캐스트
	void Deactivate();

	UPROPERTY()
	FOnMonsterDeactivated OnDeactivated;
};
