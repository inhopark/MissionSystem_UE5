#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
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
	UHPWidget* HPWidgetInstance;

	UPROPERTY()
	UMissionTimerWidget* MissionTimerWidgetInstance;

	// WidgetClass(미지정 시 FallbackPath로 로드) -> 생성 -> 뷰포트에 추가 -> 처음엔 Hidden.
	// BeginPlay에서 네 개 위젯 모두 똑같이 반복되는 생성 절차를 한 곳으로 모음
	template <typename TWidgetClass>
	TWidgetClass* CreateAndAddWidget(TSubclassOf<TWidgetClass> WidgetClass, const TCHAR* FallbackPath);

public:

	// 디펜스 모드 진입/종료 시 HP・타이머 위젯을 함께 표시/숨김
	void SetDefenseHUDVisible(bool bVisible);
};

template <typename TWidgetClass>
TWidgetClass* AMainPlayerController::CreateAndAddWidget(TSubclassOf<TWidgetClass> WidgetClass, const TCHAR* FallbackPath)
{
	TSubclassOf<TWidgetClass> ClassToUse = WidgetClass;
	if (ClassToUse == nullptr)
	{
		// 에디터/블루프린트에서 미지정 시 기존 에셋 경로로 Fallback 로드
		ClassToUse = LoadClass<TWidgetClass>(nullptr, FallbackPath);
	}

	if (ClassToUse == nullptr)
	{
		return nullptr;
	}

	TWidgetClass* NewWidget = CreateWidget<TWidgetClass>(this, ClassToUse);
	if (NewWidget != nullptr)
	{
		// 처음엔 안 보이게 설정 후 뷰포트에 추가
		NewWidget->AddToViewport();
		NewWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	return NewWidget;
}
