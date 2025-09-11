// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TFDNativeGameplayTags.h"
#include "TFDPlayerController.generated.h"


struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

/**
 * 
 */
UCLASS()
class TFD_API ATFDPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ATFDPlayerController();

	UFUNCTION(BlueprintCallable)
	void SetMovemnetWalking();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void AcknowledgePossession(APawn* InPawn) override;

private:
	void Dash(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void StopJumping();
	/*
	void Attack(const FInputActionValue& Value);
	void TogglePause(const FInputActionValue& Value);
	*/

	
	void JobAbility(const FInputActionValue& Value, FGameplayTag InputTag); //DataAsset에서 추가한 액션과 태그로 자동 바인딩

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TFD|Input|Default")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TFD|Input|Default")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TFD|Input|Default")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TFD|Input|Default")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TFD|Input")
	TObjectPtr<UInputAction> DashAction;
	
	/*
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TFD|Input")
	TObjectPtr<UInputAction> AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TFD|Input")
	TObjectPtr<UInputAction> PauseAction;
	*/


private:

	TWeakObjectPtr<UInputMappingContext> ActiveJobIMC; //OnUnPossess시 삭제할 직업 IMC 저장공간
	TArray<int32> JobBindingHandles; //OnUnPossess시 삭제할 직업 InputAction 리스트 저장공간
};
