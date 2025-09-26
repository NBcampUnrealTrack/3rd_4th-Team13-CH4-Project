#include "AnimInstance/TFDAnimInstanceBase.h"
//#include "Character/TFDCharacter.h"
#include "Character/TFDCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "TFDNativeGameplayTags.h"
#include "AbilitySystemComponent.h"

void UTFDAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ATFDCharacterBase>(GetOwningActor());
	if (IsValid(OwnerCharacter) == true)
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void UTFDAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(OwnerCharacter) == false || IsValid(OwnerCharacterMovementComponent) == false)
	{
		return;
	}
	
	// CharacterMovement Replication 이 꺼져있어서 동기화 되지 않음.
	// 호스트, 클라이언트
	Velocity = OwnerCharacterMovementComponent->GetLastUpdateVelocity();   
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.f).Size(); 
	// GetCurrentAcceleration 은 input 을 받았을 때만 변경됨
	// CharacterMovement Use Acceleration for Paths 을 사용해야 NavMesh 이동으로 인한 가속이 적용됨.
	bShouldMove = ((OwnerCharacterMovementComponent->GetCurrentAcceleration().IsNearlyZero()) == false)
		&& (3.f < GroundSpeed);
	bIsFalling = OwnerCharacterMovementComponent->IsFalling();
}

void UTFDAnimInstanceBase::PlayHitAnim(EHitDirection Direction)
{
	if (!HitMontage) return;

	//CurrentHitDirection = Direction;
	Montage_Play(HitMontage);
	// FName SectionName = GetSectionNameByDirection(Direction);
	// Montage_JumpToSection(SectionName, HitMontage);
	bIsHitPlaying = true;
}

void UTFDAnimInstanceBase::HitAnimEnd()
{
	if (bIsHitPlaying)
	{
		Montage_Stop(0.1f, HitMontage);
	}
	bIsHitPlaying = false;
	//CurrentHitDirection = EHitDirection::None;
}

FName UTFDAnimInstanceBase::GetSectionNameByDirection(EHitDirection Direction)
{
	switch(Direction)
	{
	case EHitDirection::Front:
		return FName("Front");
	case EHitDirection::Back:
		return FName("Back");
	case EHitDirection::Left:
		return FName("Left");
	case EHitDirection::Right:
		return FName("Right");
	default:
		return FName("Front"); // 기본값
	}
}