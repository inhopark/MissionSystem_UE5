#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Define/MissionDefine.h"
#include "MissionManager.generated.h"

UCLASS()
class MISSIONSYSTEM_API UMissionManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:

	UPROPERTY()
	class UMissionFactory* MissionFactory;

	UPROPERTY()
	class UBaseMission* CurrentMission;

	UPROPERTY()
	class UMainMissionWidget* MainMissionWidget;

	UPROPERTY()
	class ANPC* CurrentMissionNPC;

	UPROPERTY()
	class AMonsterSpawner* MonsterSpawner;

private:

	bool IsMissionRequest();

	void SetUICursorMode(bool bShow);

	void RequestMission(EMissionUnique eMissionUnique);

	void ClearCurrentMission();

	class AMonsterSpawner* GetOrCreateMonsterSpawner();

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	
	void RegisterMainMissionWidget(class UMainMissionWidget* Widget);

	void ShowMainMissionWidget(EMissionUnique MissionUnique, class ANPC* InNPC = nullptr);

	void HideMainMissionWidget();

	UFUNCTION()
	void HandleMissionButtonAction(EMissionButtonAction Action);

	UFUNCTION()
	void OnMissionStateChanged(EMissionState NewState);
	
};
