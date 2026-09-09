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
		Destroy();
	}
}

void AMonster::OnCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMissionSystemCharacter* pUser = Cast<AMissionSystemCharacter>(OtherActor))
	{
		pUser->ApplyDamage(DamageAmount);

		Destroy();
	}
}
