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
	class UMissionResultWidget* MissionResultWidget;

	UPROPERTY()
	class ANPC* CurrentMissionNPC;

	UPROPERTY()
	class AMonsterSpawner* MonsterSpawner;

	// 디펜스 미니게임에서 이 시간(초) 동안 생존하면 미션 성공 처리
	UPROPERTY()
	float MissionSuccessDuration = 30.0f;

	FTimerHandle MissionSuccessTimerHandle;

private:

	bool IsMissionRequest();

	void SetUICursorMode(bool bShow);

	void RequestMission(EMissionUnique eMissionUnique);

	void ClearCurrentMission();

	class AMonsterSpawner* GetOrCreateMonsterSpawner();

	// 결과창(성공/실패) 노출 및 디펜스 미니게임 종료 처리
	void ShowMissionResultWidget(EMissionState NewState);

	void StopDefenseMinigame();

	UFUNCTION()
	void HandleMissionSurvived();

	UFUNCTION()
	void HandleMissionResultConfirmed();

	// 디펜스 모드에서 보여지는 UI 위젯 Visible 여부를 설정
	void SetDefenseHUDVisible(bool bVisible);

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	
	void RegisterMainMissionWidget(class UMainMissionWidget* Widget);

	void RegisterMissionResultWidgets(class UMissionResultWidget* ResultWidget);

	void ShowMainMissionWidget(EMissionUnique MissionUnique, class ANPC* InNPC = nullptr);

	void HideMainMissionWidget();

	UFUNCTION()
	void HandleMissionButtonAction(EMissionButtonAction Action);

	UFUNCTION()
	void OnMissionStateChanged(EMissionState NewState);

	// 플레이어 HP가 0이 됐을 때: 미션 실패 처리 후 실패 결과창 노출 (초기화는 Confirm 클릭 시 처리)
	void HandlePlayerDefeated();

	// 디펜스 미니게임 생존 타이머 남은 시간(초). 타이머가 없으면 음수 반환
	float GetMissionRemainingTime() const;

	FORCEINLINE float GetMissionTotalDuration() const { return MissionSuccessDuration; }

};
