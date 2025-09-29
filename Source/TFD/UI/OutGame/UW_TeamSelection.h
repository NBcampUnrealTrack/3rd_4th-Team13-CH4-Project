#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TFDNativeGameplayTags.h"
#include "UW_TeamSelection.generated.h"

class UButton;
class UTextBlock;
class UEditableTextBox;


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

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* NicknameTextBox;




private:
	UFUNCTION()
	void OnClickTeamCop();

	UFUNCTION()
	void OnClickTeamThief();

	UFUNCTION()
	void OnNicknameChanged(const FText& Text);

	void UpdateSelectedTeamText(FGameplayTag NewTeam);

	FGameplayTag SelectedTeam;

};
