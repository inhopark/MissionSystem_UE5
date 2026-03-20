#include "MainPlayerController.h"
#include "Manager/MissionManager.h" 
#include "UMG/MainMissionWidget.h"
#include "Kismet/GameplayStatics.h"

void AMainPlayerController::BeginPlay()
{
    Super::BeginPlay();

    UClass* pMainMissionWidgetClass = LoadClass<UMainMissionWidget>(nullptr, TEXT("/Game/UMG/WBP_MainMission.WBP_MainMission_C"));
    if (pMainMissionWidgetClass)
    {
        MainMissionWidgetInstance = CreateWidget<UMainMissionWidget>(this, pMainMissionWidgetClass);

        if (MainMissionWidgetInstance)
        {
            // 처음엔 안 보이게 할 수도 있음
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