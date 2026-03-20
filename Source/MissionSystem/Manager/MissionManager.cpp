#include "MissionManager.h"
#include "Factory/MissionFactory.h"
#include "UMG/MainMissionWIdget.h"

void UMissionManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    MissionFactory = NewObject<UMissionFactory>(this);
}

void UMissionManager::RequestMission(EMissionUnique eMissionUnique)
{
    // 미션진행 중일경우 실패.
    if(IsMissionRequest() == false)
    {
        UE_LOG(LogTemp, Log, TEXT("## MissionManager::RequestMission ## Mission is already in progress. Cannot request a new mission."));
        return;
	}

    // 미션 진행 
    if (MissionFactory != nullptr)
    {
        CurrentMission = MissionFactory->CreateMission(this, eMissionUnique);
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

void UMissionManager::ShowMainMissionWidget(EMissionUnique MissionUnique)
{
    if (MainMissionWidget == nullptr)
    {
		UE_LOG(LogTemp, Log, TEXT("## MissionManager::ShowMainMissionWidget ## MainMissionWidget is not registered. Cannot show mission widget."));
        return;
    }

    // 마우스 커서 모드
    SetUICursorMode(true);

    MainMissionWidget->SetVisibility(ESlateVisibility::Visible);

    // 현재 미션 진행 중.
    if (CurrentMission == nullptr)
    {
		RequestMission(MissionUnique);
    }
}

void UMissionManager::HIdeMainMissionWidget()
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
        if (PC)
        {
            PC->bShowMouseCursor = bShow;

            PC->SetIgnoreLookInput(bShow);
        }
	}
}

void UMissionManager::HandleMissionButtonAction(EMissionButtonAction Action)
{
    switch (Action)
    {
        case EMissionButtonAction::Left:
            {
                if (CurrentMission != nullptr)
                {
                    if(CurrentMission->GetMissionState() == EMissionState::Ready)
                    {
                        // 미션 시작 처리
                        CurrentMission->AgreeMission();
                    }
                    else if(CurrentMission->GetMissionState() == EMissionState::InProgress)
                    {
                        // 미션 성공 처리.
                        CurrentMission->SuccessMission();

                        // 일단 미션 초기화 시킴.
                        ClearCurrentMission();
					}
                }
            }
			break;
        case EMissionButtonAction::Right:
            {
            if (CurrentMission != nullptr)
            {
                if (CurrentMission->GetMissionState() == EMissionState::Ready)
                {
                    // 미션 취소 처리.
                    CurrentMission->DisagreeMission();

                    // 일단 미션 초기화 시킴.
                    ClearCurrentMission();
                }
                else if (CurrentMission->GetMissionState() == EMissionState::InProgress)
                {
                    // 미션 실패 처리 
                    CurrentMission->FailedMission();

                    // 일단 미션 초기화 시킴.
                    ClearCurrentMission();
                }
            }
            }
            break;
    }   
}

void UMissionManager::OnMissionStateChanged(EMissionState NewState)
{
    if (MainMissionWidget)
    {
        MainMissionWidget->SetWidgetState(NewState);
    }
}

void UMissionManager::ClearCurrentMission()
{
    if (CurrentMission != nullptr)
    {
        CurrentMission->OnMissionStateChanged.RemoveDynamic(this, &UMissionManager::OnMissionStateChanged);

        CurrentMission = nullptr;
    }

	MainMissionWidget->SetVisibility(ESlateVisibility::Hidden);
}