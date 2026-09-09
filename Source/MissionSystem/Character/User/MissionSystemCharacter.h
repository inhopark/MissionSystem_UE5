// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MissionSystemCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UENUM(BlueprintType)
enum class ECharacterPlayMode : uint8
{
	Normal,        // 일반 3인칭 자유 이동 모드
	DefenseMode    // 쿼터뷰 디펜스 미니게임 모드
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, float, CurrentHP, float, MaxHP);

UCLASS(config=Game)
class AMissionSystemCharacter : public ACharacter
{
	GENERATED_BODY()

private:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayMode")
	ECharacterPlayMode CurrentPlayMode = ECharacterPlayMode::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Defense")
	float DefenseCameraDistance = 1300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Defense")
	float DefenseCameraForwardOffset = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
	float MaxHP = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HP")
	float CurrentHP = 100.0f;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:

	virtual void Tick(float DeltaTime) override;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void SetPlayMode(ECharacterPlayMode NewMode);
	FORCEINLINE ECharacterPlayMode GetPlayMode() const { return CurrentPlayMode; }

	// DamageAmount만큼 HP를 깎고 OnHPChanged를 브로드캐스트. HP가 0이 되면 MissionManager에 알려 미션을 초기화시킴
	void ApplyDamage(float DamageAmount);

	// HP를 MaxHP로 회복시키고 OnHPChanged를 브로드캐스트
	void ResetHP();

	FORCEINLINE float GetCurrentHP() const { return CurrentHP; }
	FORCEINLINE float GetMaxHP() const { return MaxHP; }

	UPROPERTY(BlueprintAssignable, Category = "HP")
	FOnHPChanged OnHPChanged;

public:
	AMissionSystemCharacter();
};

