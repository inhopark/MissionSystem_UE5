#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Define/MissionDefine.h"
#include "Mission/BaseMission.h"
#include "MissionFactory.generated.h"

UCLASS()
class MISSIONSYSTEM_API UMissionFactory : public UObject
{
	GENERATED_BODY()

public:

	UBaseMission* CreateMission(UObject* pOuter, EMissionUnique eMissionUnique);
	
};
