#include "MissionFactory.h"
#include "Mission/BaseMission.h"

UBaseMission* UMissionFactory::CreateMission(UObject* pOuter, EMissionUnique eMissionUnique)
{
	switch (eMissionUnique)
	{	
		case EMissionUnique::Act1Chapter1: return NewObject<UBaseMission>(pOuter);
		case EMissionUnique::Act1Chapter2: return NewObject<UBaseMission>(pOuter);
	}

	return nullptr;
}
