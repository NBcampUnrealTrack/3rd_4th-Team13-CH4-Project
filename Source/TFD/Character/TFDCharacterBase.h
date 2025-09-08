#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "Logging/LogMacros.h"
#include "TFDPlayerDataAsset.h"
#include "TFDCharacterBase.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class TFD_API ATFDCharacterBase : public ACharacter
	, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	ATFDCharacterBase();

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;


	virtual void NotifyControllerChanged() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginPlay() override;

protected:
	// GAS 핵심 컴포넌트들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	class UTFDAttributeSet* AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	class UTFDPlayerDataAsset* CharacterData;

private:
	// GAS 기본 생성자
	void BaseSetting();
	void SetDAPlayerStat();
};
