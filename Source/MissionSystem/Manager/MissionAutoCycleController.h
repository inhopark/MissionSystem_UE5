#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Define/MissionDefine.h"
#include "MissionAutoCycleController.generated.h"

class ANPC;
class AMissionSystemCharacter;
class UMissionManager;

// 플레이어 조작 없이 미션 한 사이클(NPC 접근 -> Agree -> 디펜스 미니게임 -> 결과 Confirm)을
// 자동으로 한 번 진행시키는 데모/테스트용 컨트롤러(GameInstanceSubsystem).
// UBaseMission/UDefenseMinigameMission, 각 UMG 위젯 클래스는 이 모드의 존재를 전혀 모름 —
// 이 클래스가 UMissionManager가 노출하는 public 함수(HandleMissionButtonAction 등)를
// 사람이 버튼을 누른 것과 동일한 방식으로 대신 호출해서 사이클을 진행시킴.
// FTickableGameObject를 구현하는 이유: NPC 접근/디펜스 회피 이동은 매 프레임 AddMovementInput을
// 호출해야 정상 속도로 부드럽게 움직임(타이머로 0.05초마다 한 번씩만 넣으면 그 사이 프레임에는
// 입력이 없어 감속->재가속이 반복되며 캐릭터가 덜덜거림).
UCLASS()
class MISSIONSYSTEM_API UMissionAutoCycleController : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

private:

	bool bEnabled = false;

	// NPC 방향으로 이동 중인지 (Ready 상태가 되면 꺼짐)
	bool bApproachingNPC = false;

	// 디펜스 미니게임 중 좌우 회피 이동 중인지 (Succeeded/Failed가 되면 꺼짐)
	bool bDodging = false;

	bool bDodgeMovingRight = true;

	// 현재 회피 방향으로 이동을 시작한 위치 (여기서부터 DodgeSwitchDistance만큼 움직이면 방향 전환)
	FVector DodgeLegStartLocation = FVector::ZeroVector;

	UPROPERTY()
	ANPC* TargetNPC;

	FTimerHandle ActionDelayTimerHandle;

	// NPC에게 접근할 때 적용할 이동 속도 (기본 걷기 속도보다 빠르게 해서 데모 진행을 단축).
	// 너무 크게 잡으면 Walk/Run 블렌드스페이스(BS_MF_Unarmed_WalkRun, Speed 샘플 최대 500)가
	// 캘리브레이션된 속도를 훨씬 초과해서 발이 미끄러지는 것처럼 보이므로 500 근처로 유지할 것
	UPROPERTY(EditAnywhere, Category = "AutoCycle")
	float AutoApproachMoveSpeed = 650.0f;

	// 디펜스 미니게임 중 좌/우 회피 방향을 바꾸기까지 이동할 거리(cm) — 짧을수록 자주 왔다갔다함
	UPROPERTY(EditAnywhere, Category = "AutoCycle")
	float DodgeSwitchDistance = 150.0f;

	// 미션창이 뜬 뒤(Ready) Agree 클릭까지의 딜레이(초)
	UPROPERTY(EditAnywhere, Category = "AutoCycle")
	float AutoAgreeDelay = 1.5f;

	// 결과창이 뜬 뒤(Succeeded/Failed) Confirm 클릭까지의 딜레이(초)
	UPROPERTY(EditAnywhere, Category = "AutoCycle")
	float AutoConfirmDelay = 1.5f;

	// 자동 사이클에서 디펜스 미니게임 생존 시간을 이 값으로 단축 (기본 30초 -> 5초)
	UPROPERTY(EditAnywhere, Category = "AutoCycle")
	float AutoCycleSurvivalDuration = 5.0f;

	UMissionManager* GetMissionManager() const;

	AMissionSystemCharacter* GetPlayerCharacter() const;

	ANPC* FindNearestNPC(const AMissionSystemCharacter* PlayerCharacter) const;

	void StartApproachingTargetNPC();

	void StopApproachingTargetNPC();

	void AdvanceTowardTargetNPC();

	void StartDodging();

	void StopDodging();

	void AdvanceDodgeMovement();

	UFUNCTION()
	void HandleMissionStateChanged(EMissionState NewState);

	void TriggerAutoAgree();

	void TriggerAutoConfirm();

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// FTickableGameObject
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return bApproachingNPC || bDodging; }
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UMissionAutoCycleController, STATGROUP_Tickables); }

	// 자동 사이클 모드를 켜고 끔 (AMainPlayerController::ToggleAutoCycleMode()가 콘솔 명령
	// "ToggleAutoCycleMode"을 받아 이 함수를 호출해줌 — GameInstanceSubsystem은 콘솔 Exec
	// 디스패치 체인에 포함되지 않아 직접 UFUNCTION(Exec)로 노출할 수 없음).
	// 켜지면 가장 가까운 NPC에게 자동으로 다가가 한 사이클을 진행하고, 끝나면 자동으로 꺼짐
	void ToggleAutoCycleMode();
};
