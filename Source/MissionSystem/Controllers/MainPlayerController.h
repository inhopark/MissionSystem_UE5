#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

class UMainMissionWidget;
class UMissionResultWidget;
class UHPWidget;
class UMissionTimerWidget;

UCLASS()
class MISSIONSYSTEM_API AMainPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UMainMissionWidget> MainMissionWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UMissionResultWidget> MissionResultWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UMissionResultWidget> MissionFailWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UHPWidget> HPWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UMissionTimerWidget> MissionTimerWidgetClass;

    virtual void BeginPlay() override;

private:

    UPROPERTY()
    UMainMissionWidget* MainMissionWidgetInstance;

    UPROPERTY()
    UMissionResultWidget* MissionResultWidgetInstance;

    UPROPERTY()
    UMissionResultWidget* MissionFailWidgetInstance;

    UPROPERTY()
    UHPWidget* HPWidgetInstance;

    UPROPERTY()
    UMissionTimerWidget* MissionTimerWidgetInstance;

public:

    // 디펜스 모드 진입/종료 시 HP・타이머 위젯을 함께 표시/숨김
    void SetDefenseHUDVisible(bool bVisible);
};