#include "MissionFactory.h"
#include "Mission/BaseMission.h"
#include "Mission/MissionAct1Chapter1.h"
#include "Mission/MissionAct1Chapter2.h"

UBaseMission* UMissionFactory::CreateMission(UObject* Outer, EMissionUnique MissionUnique)
{
	switch (MissionUnique)
	{
		case EMissionUnique::Act1Chapter1: return NewObject<UMissionAct1Chapter1>(Outer);
		case EMissionUnique::Act1Chapter2: return NewObject<UMissionAct1Chapter2>(Outer);
	}

	return nullptr;
}
