#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

UCLASS()
class MISSIONSYSTEM_API AMainPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

private:

    UPROPERTY()
    class UMainMissionWidget* MainMissionWidgetInstance;
};