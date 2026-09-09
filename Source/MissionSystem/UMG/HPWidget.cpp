#include "HPWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Character/User/MissionSystemCharacter.h"
#include "Kismet/GameplayStatics.h"

void UHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AMissionSystemCharacter* pUser = Cast<AMissionSystemCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
	{
		pUser->OnHPChanged.AddDynamic(this, &UHPWidget::HandleHPChanged);

		SetHP(pUser->GetCurrentHP(), pUser->GetMaxHP());

		// 위젯이 새로 생성된 시점엔 보간 없이 바로 현재 값으로 표시
		DisplayedPercent = TargetPercent;
		if (HPBar != nullptr)
		{
			HPBar->SetPercent(DisplayedPercent);
		}
	}
}

void UHPWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (HPBar != nullptr && !FMath::IsNearlyEqual(DisplayedPercent, TargetPercent, KINDA_SMALL_NUMBER))
	{
		DisplayedPercent = FMath::FInterpTo(DisplayedPercent, TargetPercent, InDeltaTime, BarInterpSpeed);
		HPBar->SetPercent(DisplayedPercent);
	}
}

void UHPWidget::HandleHPChanged(float CurrentHP, float MaxHP)
{
	SetHP(CurrentHP, MaxHP);
}

void UHPWidget::SetHP(float CurrentHP, float MaxHP)
{
	// 게이지는 목표치만 갱신 -> NativeTick에서 서서히 보간
	TargetPercent = MaxHP > 0.0f ? CurrentHP / MaxHP : 0.0f;

	if (HPText != nullptr)
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), FMath::RoundToInt(CurrentHP), FMath::RoundToInt(MaxHP))));
	}
}
