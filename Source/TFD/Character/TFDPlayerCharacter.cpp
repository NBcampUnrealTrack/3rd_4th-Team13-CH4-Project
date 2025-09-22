#include "Character/TFDPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Character/TFDPlayerDataAsset.h"
#include "Controller/TFDPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameState/TFDGameState.h"

ATFDPlayerCharacter::ATFDPlayerCharacter()
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	bReplicates = true;

	// 스킬 매니저 컴포넌트 생성
	SkillManagerComponent = CreateDefaultSubobject<UTFDSkillManagerComponent>(TEXT("SkillManagerComponent"));
}

void ATFDPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (AController* PC = GetController())
	{
		ATFDPlayerController* MyPC = Cast<ATFDPlayerController>(PC);
		if (MyPC)
		{
			if (ATFDGameState* GS = GetWorld()->GetGameState<ATFDGameState>())
			{
				// 기존 바인딩 제거 후 다시 바인딩 (중복 방지)
				GS->OnMachInProgress.RemoveDynamic(MyPC, &ATFDPlayerController::HandleMatchInProgress);
				GS->OnMatchWaitingPostMatch.RemoveDynamic(MyPC, &ATFDPlayerController::HandleMatchWaitingPostMatch);

				// 컨트롤러 함수 바인딩
				GS->OnMachInProgress.AddDynamic(MyPC, &ATFDPlayerController::HandleMatchInProgress);
				GS->OnMatchWaitingPostMatch.AddDynamic(MyPC, &ATFDPlayerController::HandleMatchWaitingPostMatch);
			}
		}
	}

	if (SkillManagerComponent && SkillManagerComponent->IsASCSetup())
	{
		UE_LOG(LogTemp, Log, TEXT("[ATFDPlayerCharacter][BeginPlay]SkillManagerComponent is initialized with ASC."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATFDPlayerCharacter][BeginPlay]SkillManagerComponent is NOT initialized or ASC is missing."));
	}
}

void ATFDPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (SkillManagerComponent)
	{
		SkillManagerComponent->SetupASC();
		UE_LOG(LogTemp, Log, TEXT("[ATFDPlayerCharacter][PossessedBy] SkillManagerComponent initialized."));
	}
}

void ATFDPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (SkillManagerComponent)
	{
		SkillManagerComponent->SetupASC();
		UE_LOG(LogTemp, Log, TEXT("[ATFDPlayerCharacter][OnRep_PlayerState] SkillManagerComponent initialized."));
	}
}

