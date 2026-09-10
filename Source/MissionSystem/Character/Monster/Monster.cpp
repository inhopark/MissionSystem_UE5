#include "Monster.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Character/User/MissionSystemCharacter.h"

AMonster::AMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(50.0f);
	CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = CollisionSphere;

	MonsterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MonsterMesh"));
	MonsterMesh->SetupAttachment(RootComponent);
	MonsterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 별도 메시를 지정하지 않은 경우를 대비한 기본 시각화(엔진 기본 구체)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (DefaultMeshAsset.Succeeded())
	{
		MonsterMesh->SetStaticMesh(DefaultMeshAsset.Object);
		MonsterMesh->SetRelativeScale3D(FVector(0.7f));
	}
}

void AMonster::BeginPlay()
{
	Super::BeginPlay();

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AMonster::OnCollisionSphereBeginOverlap);
}

void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float DeltaMove = MoveSpeed * DeltaTime;
	AddActorWorldOffset(FVector(-DeltaMove, 0.0f, 0.0f));

	DistanceTraveled += DeltaMove;
	if (DistanceTraveled >= MaxTravelDistance)
	{
		// 플레이어를 지나쳐 화면 밖으로 벗어남 -> 풀로 반환
		Deactivate();
	}
}

void AMonster::OnCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMissionSystemCharacter* PlayerCharacter = Cast<AMissionSystemCharacter>(OtherActor))
	{
		PlayerCharacter->ApplyDamage(DamageAmount);

		// 플레이어를 맞췄으니 풀로 반환
		Deactivate();
	}
}

void AMonster::ActivateAt(const FVector& SpawnLocation)
{
	SetActorLocation(SpawnLocation);
	DistanceTraveled = 0.0f;
	bIsActive = true;

	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
}

void AMonster::Deactivate()
{
	// 같은 프레임에 충돌 + 최대 이동거리 초과가 겹치는 등, 두 번 호출되어도
	// 풀에 중복 반환되지 않도록 가드
	if (bIsActive == false)
	{
		return;
	}
	bIsActive = false;

	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);

	OnDeactivated.Broadcast(this);
}
