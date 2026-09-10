// Copyright Epic Games, Inc. All Rights Reserved.

#include "MissionSystemCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Manager/MissionManager.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AMissionSystemCharacter

AMissionSystemCharacter::AMissionSystemCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = NormalModeMoveSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	CurrentHP = MaxHP;
}

void AMissionSystemCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CameraBoom != nullptr)
	{
		if (CurrentPlayMode == ECharacterPlayMode::DefenseMode)
		{
			// 쿼터뷰 카메라 거리 및 각도로 부드럽게 보간
			CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, DefenseCameraDistance, DeltaTime, 3.0f);
			const FRotator TargetRot(-55.0f, 0.0f, 0.0f);
			CameraBoom->SetRelativeRotation(FMath::RInterpTo(CameraBoom->GetRelativeRotation(), TargetRot, DeltaTime, 3.0f));

			// 시선을 앞쪽(전장 방향)으로 밀어 주인공이 화면 하단 중앙에 위치하도록 TargetOffset 보간
			const FVector TargetOffset(DefenseCameraForwardOffset, 0.0f, 0.0f);
			CameraBoom->TargetOffset = FMath::VInterpTo(CameraBoom->TargetOffset, TargetOffset, DeltaTime, 3.0f);

			// 캐릭터는 항상 월드 전방(위쪽) 응시
			SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
		else
		{
			// 일반 3인칭 거리 및 오프셋으로 부드럽게 복귀
			CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, 400.0f, DeltaTime, 4.0f);
			CameraBoom->TargetOffset = FMath::VInterpTo(CameraBoom->TargetOffset, FVector::ZeroVector, DeltaTime, 4.0f);
		}
	}
}

void AMissionSystemCharacter::SetPlayMode(ECharacterPlayMode NewMode)
{
	CurrentPlayMode = NewMode;

	if (CameraBoom != nullptr && GetCharacterMovement() != nullptr)
	{
		if (CurrentPlayMode == ECharacterPlayMode::DefenseMode)
		{
			CameraBoom->bUsePawnControlRotation = false;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->MaxWalkSpeed = DefenseModeMoveSpeed;
			SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
		else
		{
			CameraBoom->bUsePawnControlRotation = true;
			GetCharacterMovement()->bOrientRotationToMovement = true;
			GetCharacterMovement()->MaxWalkSpeed = NormalModeMoveSpeed;
		}
	}
}

void AMissionSystemCharacter::ApplyDamage(float DamageAmount)
{
	CurrentHP = FMath::Clamp(CurrentHP - DamageAmount, 0.0f, MaxHP);

	OnHPChanged.Broadcast(CurrentHP, MaxHP);

	if (CurrentHP <= 0.0f)
	{
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			if (UMissionManager* MissionManager = GameInstance->GetSubsystem<UMissionManager>())
			{
				MissionManager->HandlePlayerDefeated();
			}
		}
	}
}

void AMissionSystemCharacter::ResetHP()
{
	CurrentHP = MaxHP;

	OnHPChanged.Broadcast(CurrentHP, MaxHP);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMissionSystemCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AMissionSystemCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMissionSystemCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMissionSystemCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMissionSystemCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		if (CurrentPlayMode == ECharacterPlayMode::DefenseMode)
		{
			// 디펜스 모드: 오직 좌/우(A, D) 이동만 적용
			const FVector RightDirection(0.0f, 1.0f, 0.0f);
			AddMovementInput(RightDirection, MovementVector.X);
		}
		else
		{
			// 일반 3인칭 모드: 전후좌우 자유 이동
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}
	}
}

void AMissionSystemCharacter::Look(const FInputActionValue& Value)
{
	if (CurrentPlayMode != ECharacterPlayMode::DefenseMode)
	{
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		if (Controller != nullptr)
		{
			AddControllerYawInput(LookAxisVector.X);
			AddControllerPitchInput(LookAxisVector.Y);
		}
	}
}