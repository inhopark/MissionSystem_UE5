#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Define/MissionDefine.h"
#include "Mission/BaseMission.h"
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

	// 결과창의 Confirm 버튼(UMissionResultWidget::OnConfirmed)에 바인딩됨.
	// public인 이유: 자동 사이클 모드(UMissionAutoCycleController)가 사람이 버튼을
	// 누른 것과 동일하게 이 함수를 직접 호출해 결과창을 닫고 미션을 정리시키기 위함.
	UFUNCTION()
	void HandleMissionResultConfirmed();

	// 플레이어 HP가 0이 됐을 때: 미션 실패 처리 후 실패 결과창 노출 (초기화는 Confirm 클릭 시 처리)
	void HandlePlayerDefeated();

	// 현재 미션의 디펜스 미니게임 남은 시간(초). CurrentMission에 위임 — 미니게임이 없으면 음수 반환
	float GetMissionRemainingTime() const;

	float GetMissionTotalDuration() const;

	// 현재 진행 중인 미션 조회 (없으면 nullptr). 자동 사이클 모드 등 외부 옵저버가
	// UDefenseMinigameMission 여부를 확인하거나 상태를 조회할 때 사용
	FORCEINLINE UBaseMission* GetCurrentMission() const { return CurrentMission; }

	// 미션 상태가 바뀔 때마다(OnMissionStateChanged 처리 이후) 브로드캐스트되는 공개 이벤트.
	// MissionManager 자신의 동작은 그대로이며, 외부 시스템(자동 사이클 모드 등)이 상태 변화를
	// 관찰할 수 있도록 노출만 하는 것 — MissionManager는 이 이벤트를 구독하는 대상이 누구인지 모름
	UPROPERTY()
	FOnMissionStateChanged OnAnyMissionStateChanged;

};
