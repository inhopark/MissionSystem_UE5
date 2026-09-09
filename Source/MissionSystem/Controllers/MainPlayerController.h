#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

class UMainMissionWidget;

UCLASS()
class MISSIONSYSTEM_API AMainPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UMainMissionWidget> MainMissionWidgetClass;

    virtual void BeginPlay() override;

private:

    UPROPERTY()
    UMainMissionWidget* MainMissionWidgetInstance;
};