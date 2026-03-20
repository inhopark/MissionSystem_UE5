#include "NPC.h"
#include "Character/User/MissionSystemCharacter.h"
#include "Components/SphereComponent.h"
#include "Manager/MissionManager.h"

ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = false;

	MissionUnique = EMissionUnique::Act1Chapter1;
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();

	if (USphereComponent* pSphereComp = FindComponentByClass<USphereComponent>())
	{
		pSphereComp->OnComponentBeginOverlap.AddDynamic(this, &ANPC::OnUserCheckOverlapBegin);
		pSphereComp->OnComponentEndOverlap.AddDynamic(this, &ANPC::OnUserCheckOverlapEnd);
	}
}

void ANPC::OnUserCheckOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMissionSystemCharacter* pUser = Cast<AMissionSystemCharacter>(OtherActor))
	{
		// 미션 요청 UI 띄우는 로직.
		if(GetGameInstance())
		{
			if (UMissionManager* pMissionManager = GetGameInstance()->GetSubsystem<UMissionManager>())
			{
				pMissionManager->ShowMainMissionWidget(MissionUnique);
			}
		}
	}
}

void ANPC::OnUserCheckOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AMissionSystemCharacter* pUser = Cast<AMissionSystemCharacter>(OtherActor))
	{
		// 미션 요청 UI 제거하는 로직.
		if(GetGameInstance())
		{
			if (UMissionManager* pMissionManager = GetGameInstance()->GetSubsystem<UMissionManager>())
			{
				pMissionManager->HIdeMainMissionWidget();
			}
		}
	}
}