#include "MainPlayerController.h"
#include "Manager/MissionManager.h"
#include "UMG/MainMissionWidget.h"
#include "UMG/MissionResultWidget.h"
#include "UMG/HPWidget.h"
#include "UMG/MissionTimerWidget.h"
#include "Kismet/GameplayStatics.h"

void AMainPlayerController::BeginPlay()
{
    Super::BeginPlay();

    TSubclassOf<UMainMissionWidget> WidgetClassToUse = MainMissionWidgetClass;
    if (WidgetClassToUse == nullptr)
    {
        // 에디터/블루프린트에서 미지정 시 기존 에셋 경로로 Fallback 로드
        WidgetClassToUse = LoadClass<UMainMissionWidget>(nullptr, TEXT("/Game/UMG/WBP_MainMission.WBP_MainMission_C"));
    }

    if (WidgetClassToUse != nullptr)
    {
        MainMissionWidgetInstance = CreateWidget<UMainMissionWidget>(this, WidgetClassToUse);

        if (MainMissionWidgetInstance != nullptr)
        {
            // 처음엔 안 보이게 설정 후 뷰포트에 추가
            MainMissionWidgetInstance->AddToViewport();
            MainMissionWidgetInstance->SetVisibility(ESlateVisibility::Hidden);

            // Subsystem에 등록
            if (UMissionManager* pMissionManager = GetGameInstance()->GetSubsystem<UMissionManager>())
            {
                pMissionManager->RegisterMainMissionWidget(MainMissionWidgetInstance);
            }
        }
    }

    TSubclassOf<UMissionResultWidget> ResultWidgetClassToUse = MissionResultWidgetClass;
    if (ResultWidgetClassToUse == nullptr)
    {
        // 에디터/블루프린트에서 미지정 시 기존 에셋 경로로 Fallback 로드
        ResultWidgetClassToUse = LoadClass<UMissionResultWidget>(nullptr, TEXT("/Game/UMG/WBP_MainMissionResult.WBP_MainMissionResult_C"));
    }

    if (ResultWidgetClassToUse != nullptr)
    {
        MissionResultWidgetInstance = CreateWidget<UMissionResultWidget>(this, ResultWidgetClassToUse);

        if (MissionResultWidgetInstance != nullptr)
        {
            MissionResultWidgetInstance->AddToViewport();
            MissionResultWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    // 결과창을 Subsystem에 등록
    if (UMissionManager* pMissionManager = GetGameInstance()->GetSubsystem<UMissionManager>())
    {
        pMissionManager->RegisterMissionResultWidgets(MissionResultWidgetInstance);
    }

    TSubclassOf<UHPWidget> HPWidgetClassToUse = HPWidgetClass;
    if (HPWidgetClassToUse == nullptr)
    {
        // 에디터/블루프린트에서 미지정 시 기존 에셋 경로로 Fallback 로드
        HPWidgetClassToUse = LoadClass<UHPWidget>(nullptr, TEXT("/Game/UMG/WBP_HP.WBP_HP_C"));
    }

    if (HPWidgetClassToUse != nullptr)
    {
        HPWidgetInstance = CreateWidget<UHPWidget>(this, HPWidgetClassToUse);

        if (HPWidgetInstance != nullptr)
        {
            // 처음엔 안 보이게 설정 후 뷰포트에 추가, 미션 시작 시 노출
            HPWidgetInstance->AddToViewport();
            HPWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    TSubclassOf<UMissionTimerWidget> TimerWidgetClassToUse = MissionTimerWidgetClass;
    if (TimerWidgetClassToUse == nullptr)
    {
        // 에디터/블루프린트에서 미지정 시 기존 에셋 경로로 Fallback 로드
        TimerWidgetClassToUse = LoadClass<UMissionTimerWidget>(nullptr, TEXT("/Game/UMG/WBP_MissionTimer.WBP_MissionTimer_C"));
    }

    if (TimerWidgetClassToUse != nullptr)
    {
        MissionTimerWidgetInstance = CreateWidget<UMissionTimerWidget>(this, TimerWidgetClassToUse);

        if (MissionTimerWidgetInstance != nullptr)
        {
            // 처음엔 안 보이게 설정 후 뷰포트에 추가, 미션 시작 시 노출
            MissionTimerWidgetInstance->AddToViewport();
            MissionTimerWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void AMainPlayerController::SetDefenseHUDVisible(bool bVisible)
{
    // 버튼이 없는 순수 표시용 위젯들이므로 HitTestInvisible로 노출해
    // 다른 위젯(미션 결과창 등)의 클릭을 가로채지 않도록 함
    const ESlateVisibility TargetVisibility = bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden;

    if (HPWidgetInstance != nullptr)
    {
        HPWidgetInstance->SetVisibility(TargetVisibility);
    }

    if (MissionTimerWidgetInstance != nullptr)
    {
        MissionTimerWidgetInstance->SetVisibility(TargetVisibility);
    }
}