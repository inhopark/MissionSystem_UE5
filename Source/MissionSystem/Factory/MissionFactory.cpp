#include "MissionFactory.h"
#include "Mission/MissionAct1Chapter1.h"
#include "Mission/MissionAct1Chapter2.h"

UBaseMission* UMissionFactory::CreateMission(UObject* pOuter, EMissionUnique eMissionUnique)
{
	switch (eMissionUnique)
	{	
		case EMissionUnique::Act1Chapter1: return NewObject<UMissionAct1Chapter1>(pOuter);
		case EMissionUnique::Act1Chapter2: return NewObject<UMissionAct1Chapter2>(pOuter);
	}

	return nullptr;
}
