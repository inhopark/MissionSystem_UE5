#pragma once

#include "CoreMinimal.h"
#include "MissionDefine.generated.h"  

UENUM(BlueprintType)
enum class EMissionUnique : uint8
{
	Act1Chapter1,
	Act1Chapter2,
};

UENUM(BlueprintType)
enum class EMissionState : uint8
{
	Ready,
	InProgress,
	Succeeded,
	Failed,
};

UENUM(BlueprintType)
enum class EMissionButtonAction : uint8
{
	Left,
	Right
};

#define LOCTEXT_NAMESPACE "MissionSystem"

inline const FText WAIT_FOR_MISSION				= LOCTEXT("WaitForMission", "Waiting for Mission");
inline const FText MISSION_IN_PROGRESS			= LOCTEXT("MissionInProgress", "Mission in Progress");
inline const FText MISSION_SUCCESS				= LOCTEXT("MissionSuccess", "Mission Success");
inline const FText MISSION_FAILED                = LOCTEXT("MissionFailed", "Mission Failed");
inline const FText MISSION_SUCCESS_BUTTON_TEXT   = LOCTEXT("MissionSuccessButton", "Success");
inline const FText MISSION_FAILED_BUTTON_TEXT    = LOCTEXT("MissionFailButton", "Fail");
inline const FText MISSION_AGREE_BUTTON_TEXT     = LOCTEXT("MissionAgreeButton", "Agree");
inline const FText MISSION_DISAGREE_BUTTON_TEXT  = LOCTEXT("MissionDisagreeButton", "Disagree");
inline const FText MISSION_CONFIRM_BUTTON_TEXT   = LOCTEXT("MissionConfirmButton", "Confirm");

#undef LOCTEXT_NAMESPACE