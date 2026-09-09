#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Monster.generated.h"

class UStaticMeshComponent;
class USphereComponent;

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
	float MoveSpeed = 400.0f;

	// 이 거리만큼 이동하면(플레이어를 지나쳤다고 판단) 자동 소멸
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float MaxTravelDistance = 3000.0f;

private:

	float DistanceTraveled = 0.0f;

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
