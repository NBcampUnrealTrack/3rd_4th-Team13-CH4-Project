#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TFDNativeGameplayTags.h"
#include "UW_TeamSelection.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class TFD_API UUW_TeamSelection : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Cop;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Thief;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Cop;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Thief;

private:
	UFUNCTION()
	void OnClickTeamCop();

	UFUNCTION()
	void OnClickTeamThief();

	void UpdateSelectedTeamText(FGameplayTag NewTeam);
};
