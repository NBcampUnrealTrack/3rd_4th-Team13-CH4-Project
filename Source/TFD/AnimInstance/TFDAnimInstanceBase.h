#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EHitDirection.h"
#include "TFDAnimInstanceBase.generated.h"

//class ATFDCharacter;
class ATFDCharacterBase;
class UCharacterMovementComponent;

UCLASS()
class TFD_API UTFDAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	void PlayHitAnim(EHitDirection Direction);
	void HitAnimEnd();

	void PlayDemeritAnim();
	void DemeritAnimEnd();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	UAnimMontage* HitMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DemeritMontage;

	static FName GetSectionNameByDirection(EHitDirection Direction);
	
protected:
	UPROPERTY()
	TObjectPtr<ATFDCharacterBase> OwnerCharacter; //TFDCharacterBase

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bShouldMove : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsFalling : 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation")
	EHitDirection CurrentHitDirection = EHitDirection::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation")
	bool bIsHitPlaying = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsDemeritPlaying = false;
};
