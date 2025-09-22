// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TFDCharacterBase.h"
#include "GameAbilitySystem/Component/TFDSkillManagerComponent.h" // 스킬 매니저 컴포넌트

#include "TFDPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class TFD_API ATFDPlayerCharacter : public ATFDCharacterBase
{
	GENERATED_BODY()
	
public:
	ATFDPlayerCharacter();

	virtual void BeginPlay() override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/*
		스킬 매니저 컴포넌트 관련
		
		테스트 중 확인 내용은 다음과 같음.
		BeginPlay에서 이미 SkillManagerComponent가 ASC 갖고오기를 성공적으로 함.
		BeginPlay 시점에 PlayerState가 유효해서 가능했음.

		이게 항상 보장되는가?
		네트워크 환경이나 리플리케이션 타이밍에 따라
		BeginPlay 시점에 PlayerState가 없거나 완전하지 않은 경우가 있을 수 있음.
		따라서 PossessedBy와 OnRep_PlayerState에서도
		ASC 초기화를 시도하는 안전장치로 두는 경우가 많다고 하여,
		예외 상황을 대비.
	*/
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

protected:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	// 스킬 매니저 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	UTFDSkillManagerComponent* SkillManagerComponent;
};
