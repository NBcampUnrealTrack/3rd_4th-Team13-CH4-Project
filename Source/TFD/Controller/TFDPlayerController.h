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
	virtual void AcknowledgePossession(APawn* InPawn) override;

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
	void Dash(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void StopJumping();
	/*
	void Attack(const FInputActionValue& Value);
	void TogglePause(const FInputActionValue& Value);
	*/

	void JobAbility(const FInputActionValue& Value, FGameplayTag InputTag);
};
