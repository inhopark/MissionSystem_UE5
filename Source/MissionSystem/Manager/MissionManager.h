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

private:

	bool IsMissionRequest();

	void SetUICursorMode(bool bShow);

	void RequestMission(EMissionUnique MissionUnique);

	void ClearCurrentMission();

	// 결과창(성공/실패) 노출
	void ShowMissionResultWidget(EMissionState NewState);

	UFUNCTION()
	void HandleMissionResultConfirmed();

	// 디펜스 모드에서 보여지는 UI 위젯 Visible 여부를 설정
	void SetDefenseHUDVisible(bool bVisible);

	// 현재 컨트롤 중인 플레이어 캐릭터 / 컨트롤러 조회 헬퍼
	class AMissionSystemCharacter* GetMissionCharacter() const;
	class AMainPlayerController* GetMainPlayerController() const;

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

	// 현재 미션의 디펜스 미니게임 남은 시간(초). CurrentMission에 위임 — 미니게임이 없으면 음수 반환
	float GetMissionRemainingTime() const;

	float GetMissionTotalDuration() const;

};
