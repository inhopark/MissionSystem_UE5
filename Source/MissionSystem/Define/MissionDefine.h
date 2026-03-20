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
	Completed,
};

UENUM(BlueprintType)
enum class EMissionButtonAction : uint8
{
	Left,
	Right
};

const FString WAIT_FOR_MISSION				= TEXT("Waiting for Mission");
const FString MISSION_IN_PROGRESS			= TEXT("Mission in Progress");
const FString MISSION_COMPLETED				= TEXT("Mission Completed");
const FString MISSION_SUCCESS				= TEXT("Mission Success");
const FString MISSION_FAILED                = TEXT("Mission Failed"); 
const FString MISSION_SUCCESS_BUTTON_TEXT   = TEXT("Success");
const FString MISSION_FAILED_BUTTON_TEXT    = TEXT("Fail");
const FString MISSION_AGREE_BUTTON_TEXT     = TEXT("Agree");
const FString MISSION_DISAGREE_BUTTON_TEXT  = TEXT("Disagree");