#include "MainPlayerController.h"
#include "Manager/MissionManager.h" 
#include "UMG/MainMissionWidget.h"
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
}