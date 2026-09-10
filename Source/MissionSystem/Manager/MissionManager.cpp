#include "MissionManager.h"
#include "Factory/MissionFactory.h"
#include "UMG/MainMissionWidget.h"
#include "UMG/MissionResultWidget.h"
#include "Character/User/MissionSystemCharacter.h"
#include "Character/NPC/NPC.h"
#include "Controllers/MainPlayerController.h"
#include "GameFramework/GameModeBase.h"

void UMissionManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	MissionFactory = NewObject<UMissionFactory>(this);
}

void UMissionManager::RequestMission(EMissionUnique MissionUnique)
{
	// 미션진행 중일경우 실패.
	if (IsMissionRequest() == false)
	{
		UE_LOG(LogTemp, Log, TEXT("## MissionManager::RequestMission ## Mission is already in progress. Cannot request a new mission."));
		return;
	}

	// 미션 진행
	if (MissionFactory != nullptr)
	{
		CurrentMission = MissionFactory->CreateMission(this, MissionUnique);
		if (CurrentMission != nullptr)
		{
			// 미션 상태 변경 이벤트 바인딩
			CurrentMission->OnMissionStateChanged.AddDynamic(this, &UMissionManager::OnMissionStateChanged);

			// 미션 초기화
			CurrentMission->Initialize();
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("## MissionManager::RequestMission ## Mission Factory is not initialized. Cannot create mission."));
	}
}

bool UMissionManager::IsMissionRequest()
{
	// 이 곳에 추후 미션 요청 가능한 조건 추가 예정.
	return CurrentMission == nullptr;
}

void UMissionManager::RegisterMainMissionWidget(UMainMissionWidget* Widget)
{
	MainMissionWidget = Widget;

	// 버튼 콜백 함수 등록.
	if (MainMissionWidget != nullptr)
	{
		MainMissionWidget->OnMissionButtonAction.AddDynamic(this, &UMissionManager::HandleMissionButtonAction);
	}
}

void UMissionManager::RegisterMissionResultWidgets(UMissionResultWidget* ResultWidget)
{
	MissionResultWidget = ResultWidget;

	// 확인 버튼 콜백 함수 등록.
	if (ResultWidget != nullptr)
	{
		ResultWidget->OnConfirmed.AddDynamic(this, &UMissionManager::HandleMissionResultConfirmed);
	}
}

void UMissionManager::ShowMainMissionWidget(EMissionUnique MissionUnique, ANPC* InNPC)
{
	if (MainMissionWidget == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("## MissionManager::ShowMainMissionWidget ## MainMissionWidget is not registered. Cannot show mission widget."));
		return;
	}

	// 이미 미션이 진행 중이면 대화창을 띄우지 않음
	if (CurrentMission != nullptr)
	{
		if (CurrentMission->GetMissionState() == EMissionState::InProgress)
		{
			return;
		}
	}

	CurrentMissionNPC = InNPC;

	// 마우스 커서 모드
	SetUICursorMode(true);

	MainMissionWidget->SetVisibility(ESlateVisibility::Visible);

	// 현재 미션 진행 중.
	if (CurrentMission == nullptr)
	{
		RequestMission(MissionUnique);
	}
}

void UMissionManager::HideMainMissionWidget()
{
	// 마우스 커서 모드 해제
	SetUICursorMode(false);

	if (MainMissionWidget != nullptr)
	{
		MainMissionWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMissionManager::SetUICursorMode(bool bShow)
{
	if (UWorld* World = GetWorld())
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if (PC != nullptr)
		{
			PC->bShowMouseCursor = bShow;
			PC->SetIgnoreLookInput(bShow);

			// 미션 창 열릴 때(bShow == true) 이동 잠금, 닫힐 때(bShow == false) 이동 재개
			PC->SetIgnoreMoveInput(bShow);
		}
	}
}

void UMissionManager::HandleMissionButtonAction(EMissionButtonAction Action)
{
	if (CurrentMission == nullptr)
	{
		return;
	}

	// Ready 상태에서만 Left/Right = Agree/Disagree로 처리. InProgress 중에는 버튼으로
	// 강제 종료하지 않음 — 미션은 오직 디펜스 미니게임 결과(생존 성공/HP 0)로만 끝남.
	switch (Action)
	{
		case EMissionButtonAction::Left:
			{
				if (CurrentMission->GetMissionState() == EMissionState::Ready)
				{
					// 미션 시작 시 NPC 화면에서 사라지게 처리 (숨김 및 충돌 끄기)
					if (CurrentMissionNPC != nullptr)
					{
						CurrentMissionNPC->SetActorHiddenInGame(true);
						CurrentMissionNPC->SetActorEnableCollision(false);
					}

					// 플레이어 쿼터뷰 디펜스 모드로 전환
					AMissionSystemCharacter* MissionCharacter = GetMissionCharacter();
					if (MissionCharacter != nullptr)
					{
						MissionCharacter->SetPlayMode(ECharacterPlayMode::DefenseMode);
					}

					// 미션 시작 처리. 디펜스 미니게임을 갖는 미션(UDefenseMinigameMission)이라면
					// 이 안에서(EnterInProgressState) 몬스터 스폰/생존 타이머가 알아서 시작됨
					CurrentMission->AgreeMission(MissionCharacter);

					SetDefenseHUDVisible(true);

					// 미션 대화창 닫고 마우스 숨기며 이동(좌우) 활성화
					HideMainMissionWidget();
				}
			}
			break;
		case EMissionButtonAction::Right:
			{
				if (CurrentMission->GetMissionState() == EMissionState::Ready)
				{
					// 미션 취소 처리.
					CurrentMission->DisagreeMission();

					// 일단 미션 초기화 시킴.
					ClearCurrentMission();
				}
			}
			break;
	}
}

void UMissionManager::OnMissionStateChanged(EMissionState NewState)
{
	switch (NewState)
	{
		case EMissionState::Ready:
		case EMissionState::InProgress:
			if (MainMissionWidget != nullptr)
			{
				MainMissionWidget->SetWidgetState(NewState);
			}
			break;
		case EMissionState::Succeeded:
		case EMissionState::Failed:
			// 3인칭 모드 복귀 전, 디펜스 모드 종료 시 HP/타이머 위젯 다시 숨김
			SetDefenseHUDVisible(false);

			// 결과 창
			ShowMissionResultWidget(NewState);
			break;
	}
}

void UMissionManager::ShowMissionResultWidget(EMissionState NewState)
{
	if (MissionResultWidget == nullptr)
	{
		return;
	}

	SetUICursorMode(true);

	MissionResultWidget->SetResultState(NewState);
	MissionResultWidget->SetVisibility(ESlateVisibility::Visible);
}

void UMissionManager::HandleMissionResultConfirmed()
{
	if (MissionResultWidget != nullptr)
	{
		MissionResultWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	// 체력을 회복시켜 다음 미션은 처음 상태로 시작하도록 함
	if (AMissionSystemCharacter* MissionCharacter = GetMissionCharacter())
	{
		MissionCharacter->ResetHP();
	}

	// NPC 복귀, 디펜스 모드/스폰 종료, 위젯 정리 등 처음 상태로 리셋
	ClearCurrentMission();
}

void UMissionManager::ClearCurrentMission()
{
	if (CurrentMission != nullptr)
	{
		CurrentMission->OnMissionStateChanged.RemoveDynamic(this, &UMissionManager::OnMissionStateChanged);

		CurrentMission = nullptr;
	}

	// 플레이어를 PlayerStart 위치로 되돌리고 3인칭 모드로 복귀.
	// NPC를 다시 활성화하기 전에 이동시켜, 같은 자리에서 NPC와 바로 재충돌(재오버랩)하는 버그를 방지함
	if (AMissionSystemCharacter* MissionCharacter = GetMissionCharacter())
	{
		if (UWorld* World = GetWorld())
		{
			if (AGameModeBase* GameMode = World->GetAuthGameMode())
			{
				if (APlayerController* PC = World->GetFirstPlayerController())
				{
					if (AActor* PlayerStart = GameMode->FindPlayerStart(PC))
					{
						MissionCharacter->SetActorLocationAndRotation(PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), false, nullptr, ETeleportType::TeleportPhysics);
					}
				}
			}
		}

		MissionCharacter->SetPlayMode(ECharacterPlayMode::Normal);
	}

	// 미션 종료 시 숨겨졌던 NPC 다시 등장
	if (CurrentMissionNPC != nullptr)
	{
		CurrentMissionNPC->SetActorHiddenInGame(false);
		CurrentMissionNPC->SetActorEnableCollision(true);
		CurrentMissionNPC = nullptr;
	}

	HideMainMissionWidget();
}

void UMissionManager::HandlePlayerDefeated()
{
	// 진행 중이던 미션을 실패 처리 -> 실패 결과창 노출. 초기 상태로의 리셋은
	// HandleMissionResultConfirmed에서 플레이어가 확인 버튼을 누를 때 처리됨.
	// (디펜스 미니게임 정지는 미션 자신의 EnterFailedState()에서 동기적으로 처리되므로
	// 사망 즉시 추가 피해가 들어가지 않음)
	if (CurrentMission != nullptr)
	{
		CurrentMission->FailedMission();
	}
}

float UMissionManager::GetMissionRemainingTime() const
{
	return CurrentMission != nullptr ? CurrentMission->GetMinigameRemainingTime() : -1.0f;
}

float UMissionManager::GetMissionTotalDuration() const
{
	return CurrentMission != nullptr ? CurrentMission->GetMinigameTotalDuration() : 0.0f;
}

void UMissionManager::SetDefenseHUDVisible(bool bVisible)
{
	if (AMainPlayerController* MainPC = GetMainPlayerController())
	{
		MainPC->SetDefenseHUDVisible(bVisible);
	}
}

AMissionSystemCharacter* UMissionManager::GetMissionCharacter() const
{
	if (AMainPlayerController* MainPC = GetMainPlayerController())
	{
		return Cast<AMissionSystemCharacter>(MainPC->GetPawn());
	}

	return nullptr;
}

AMainPlayerController* UMissionManager::GetMainPlayerController() const
{
	if (const UWorld* World = GetWorld())
	{
		return Cast<AMainPlayerController>(World->GetFirstPlayerController());
	}

	return nullptr;
}
