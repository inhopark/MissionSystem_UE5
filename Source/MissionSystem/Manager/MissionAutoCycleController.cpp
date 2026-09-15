#include "MissionAutoCycleController.h"
#include "MissionManager.h"
#include "DefenseMinigameController.h"
#include "Mission/BaseMission.h"
#include "Mission/DefenseMinigameMission.h"
#include "Character/NPC/NPC.h"
#include "Character/User/MissionSystemCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UMissionAutoCycleController::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UMissionManager* MissionManager = GetMissionManager())
	{
		MissionManager->OnAnyMissionStateChanged.AddDynamic(this, &UMissionAutoCycleController::HandleMissionStateChanged);
	}
}

void UMissionAutoCycleController::Tick(float DeltaTime)
{
	if (bApproachingNPC)
	{
		AdvanceTowardTargetNPC();
	}

	if (bDodging)
	{
		AdvanceDodgeMovement();
	}
}

void UMissionAutoCycleController::ToggleAutoCycleMode()
{
	bEnabled = bEnabled == false;

	UE_LOG(LogTemp, Log, TEXT("## MissionAutoCycleController::ToggleAutoCycleMode ## AutoCycleMode = %s"), bEnabled ? TEXT("ON") : TEXT("OFF"));

	if (bEnabled)
	{
		StartApproachingTargetNPC();
	}
	else
	{
		StopApproachingTargetNPC();
		StopDodging();

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(ActionDelayTimerHandle);
		}
	}
}

void UMissionAutoCycleController::StartApproachingTargetNPC()
{
	AMissionSystemCharacter* PlayerCharacter = GetPlayerCharacter();
	if (PlayerCharacter == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("## MissionAutoCycleController::StartApproachingTargetNPC ## Player character not found."));
		bEnabled = false;
		return;
	}

	TargetNPC = FindNearestNPC(PlayerCharacter);
	if (TargetNPC == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("## MissionAutoCycleController::StartApproachingTargetNPC ## No NPC found in the level."));
		bEnabled = false;
		return;
	}

	// 데모 진행 속도를 위해 접근 중에는 일반 걷기 속도보다 빠르게 이동시킴.
	// Agree 처리 시 SetPlayMode(DefenseMode)가, 미션 종료 시 SetPlayMode(Normal)이
	// MaxWalkSpeed를 각 모드 값으로 다시 덮어쓰므로 별도로 복원할 필요는 없음
	if (UCharacterMovementComponent* MovementComponent = PlayerCharacter->GetCharacterMovement())
	{
		MovementComponent->MaxWalkSpeed = AutoApproachMoveSpeed;
	}

	bApproachingNPC = true;
}

void UMissionAutoCycleController::StopApproachingTargetNPC()
{
	bApproachingNPC = false;
}

void UMissionAutoCycleController::AdvanceTowardTargetNPC()
{
	AMissionSystemCharacter* PlayerCharacter = GetPlayerCharacter();
	if (PlayerCharacter == nullptr || TargetNPC == nullptr)
	{
		StopApproachingTargetNPC();
		return;
	}

	const FVector Direction = (TargetNPC->GetActorLocation() - PlayerCharacter->GetActorLocation()).GetSafeNormal2D();

	// bForce=true: 미션 UI가 열려있는 동안(SetUICursorMode -> SetIgnoreMoveInput)에는 사람의 이동 입력을
	// 막는 게 맞지만, 자동 사이클은 그 UI 규칙과 무관하게 항상 이동할 수 있어야 함
	PlayerCharacter->AddMovementInput(Direction, 1.0f, true);
}

void UMissionAutoCycleController::StartDodging()
{
	bDodging = true;
	bDodgeMovingRight = true;

	if (AMissionSystemCharacter* PlayerCharacter = GetPlayerCharacter())
	{
		DodgeLegStartLocation = PlayerCharacter->GetActorLocation();
	}
}

void UMissionAutoCycleController::StopDodging()
{
	bDodging = false;
}

void UMissionAutoCycleController::AdvanceDodgeMovement()
{
	AMissionSystemCharacter* PlayerCharacter = GetPlayerCharacter();
	if (PlayerCharacter == nullptr)
	{
		StopDodging();
		return;
	}

	// 디펜스 모드는 월드 Y축(오른쪽)만 좌우 이동으로 취급함(AMissionSystemCharacter::Move() 참고).
	// 시간이 아니라 현재 방향으로 실제 이동한 거리가 DodgeSwitchDistance에 도달하면 방향 전환
	const float TraveledDistance = FMath::Abs(PlayerCharacter->GetActorLocation().Y - DodgeLegStartLocation.Y);
	if (TraveledDistance >= DodgeSwitchDistance)
	{
		bDodgeMovingRight = bDodgeMovingRight == false;
		DodgeLegStartLocation = PlayerCharacter->GetActorLocation();
	}

	const FVector RightDirection(0.0f, 1.0f, 0.0f);
	const float Scale = bDodgeMovingRight ? 1.0f : -1.0f;

	PlayerCharacter->AddMovementInput(RightDirection, Scale, true);
}

void UMissionAutoCycleController::HandleMissionStateChanged(EMissionState NewState)
{
	if (bEnabled == false)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	switch (NewState)
	{
		case EMissionState::Ready:
			// 오버랩으로 미션창이 떴다는 뜻이므로 더 이상 이동할 필요 없음
			StopApproachingTargetNPC();
			World->GetTimerManager().SetTimer(ActionDelayTimerHandle, this, &UMissionAutoCycleController::TriggerAutoAgree, AutoAgreeDelay, false);
			break;
		case EMissionState::InProgress:
			// 디펜스 미니게임 시작 -> 좌우 회피 이동 시작
			StartDodging();
			break;
		case EMissionState::Succeeded:
		case EMissionState::Failed:
			StopDodging();
			World->GetTimerManager().SetTimer(ActionDelayTimerHandle, this, &UMissionAutoCycleController::TriggerAutoConfirm, AutoConfirmDelay, false);
			break;
		default:
			break;
	}
}

void UMissionAutoCycleController::TriggerAutoAgree()
{
	UMissionManager* MissionManager = GetMissionManager();
	if (MissionManager == nullptr)
	{
		return;
	}

	// Start() 호출(EnterInProgressState) 전에 생존 시간을 미리 단축 설정해둠
	if (UDefenseMinigameMission* DefenseMission = Cast<UDefenseMinigameMission>(MissionManager->GetCurrentMission()))
	{
		DefenseMission->GetOrCreateDefenseMinigameController()->SetSurvivalDurationOverride(AutoCycleSurvivalDuration);
	}

	MissionManager->HandleMissionButtonAction(EMissionButtonAction::Left);
}

void UMissionAutoCycleController::TriggerAutoConfirm()
{
	if (UMissionManager* MissionManager = GetMissionManager())
	{
		MissionManager->HandleMissionResultConfirmed();
	}

	TargetNPC = nullptr;

	// 한 사이클(NPC 접근 -> Agree -> 디펜스 -> Confirm)만 돌고 자동으로 꺼짐
	bEnabled = false;
}

UMissionManager* UMissionAutoCycleController::GetMissionManager() const
{
	return GetGameInstance() != nullptr ? GetGameInstance()->GetSubsystem<UMissionManager>() : nullptr;
}

AMissionSystemCharacter* UMissionAutoCycleController::GetPlayerCharacter() const
{
	return Cast<AMissionSystemCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

ANPC* UMissionAutoCycleController::FindNearestNPC(const AMissionSystemCharacter* PlayerCharacter) const
{
	if (PlayerCharacter == nullptr)
	{
		return nullptr;
	}

	TArray<AActor*> FoundNPCs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPC::StaticClass(), FoundNPCs);

	ANPC* Nearest = nullptr;
	float NearestDistSq = 0.0f;

	for (AActor* Actor : FoundNPCs)
	{
		ANPC* NPC = Cast<ANPC>(Actor);
		if (NPC == nullptr)
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(PlayerCharacter->GetActorLocation(), NPC->GetActorLocation());
		if (Nearest == nullptr || DistSq < NearestDistSq)
		{
			NearestDistSq = DistSq;
			Nearest = NPC;
		}
	}

	return Nearest;
}
