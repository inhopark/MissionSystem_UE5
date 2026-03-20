#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Define/MissionDefine.h"
#include "NPC.generated.h"

UCLASS()
class MISSIONSYSTEM_API ANPC : public AActor
{
	GENERATED_BODY()

private:

	EMissionUnique MissionUnique;

protected:

	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnUserCheckOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnUserCheckOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	ANPC();

};
