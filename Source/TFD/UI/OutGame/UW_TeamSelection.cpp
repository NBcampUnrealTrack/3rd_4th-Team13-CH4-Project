#include "UW_TeamSelection.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Controller/TFDPlayerController.h"
#include "GameInstance/TFDGameInstance.h"
#include "PlayerState/TFDPlayerState.h"
#include "Utility/TFDBGMSubsystem.h"


void UUW_TeamSelection::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Cop)
	{
		Btn_Cop->OnClicked.AddDynamic(this, &UUW_TeamSelection::OnClickTeamCop);
	}
	if (Btn_Thief)
	{
		Btn_Thief->OnClicked.AddDynamic(this, &UUW_TeamSelection::OnClickTeamThief);
	}
	if (NicknameTextBox)
	{
		NicknameTextBox->OnTextChanged.AddDynamic(this, &UUW_TeamSelection::OnNicknameChanged);
	}
}

void UUW_TeamSelection::OnClickTeamCop()
{
	if (ATFDPlayerController* PC = Cast<ATFDPlayerController>(GetOwningPlayer()))
	{
		UTFDGameInstance* TFD_GI = Cast<UTFDGameInstance>(GetGameInstance());
		UTFDBGMSubsystem* TFDBGMSubSyetem = TFD_GI->GetSubsystem<UTFDBGMSubsystem>();
		if (IsValid(TFDBGMSubSyetem))
		{
			TFDBGMSubSyetem->PlayUISound(EUISoundType::Click_00);
		}

		PC->SendPreferredTeam(TAG_Team_Cop);
		SelectedTeam = TAG_Team_Cop;
		UpdateSelectedTeamText(TAG_Team_Cop);
	}
}

void UUW_TeamSelection::OnClickTeamThief()
{
	if (ATFDPlayerController* PC = Cast<ATFDPlayerController>(GetOwningPlayer()))
	{
		PC->SendPreferredTeam(TAG_Team_Thief);
		SelectedTeam = TAG_Team_Thief;
		UpdateSelectedTeamText(TAG_Team_Thief);
	}
}

void UUW_TeamSelection::OnNicknameChanged(const FText& Text)
{
	if (ATFDPlayerController* PC = Cast<ATFDPlayerController>(GetOwningPlayer()))
	{
		if (ATFDPlayerState* PS = Cast<ATFDPlayerState>(PC->PlayerState))
		{
			PS->ServerSetNickname(Text.ToString()); // ������ �г��� ����
		}
	}
}


void UUW_TeamSelection::UpdateSelectedTeamText(FGameplayTag NewTeam)
{
	if (NewTeam == TAG_Team_Cop && Txt_Cop)
	{
		Txt_Cop->SetText(FText::FromString("Cop"));
		if (Txt_Thief)
			Txt_Thief->SetText(FText::GetEmpty());
	}
	else if (NewTeam == TAG_Team_Thief && Txt_Thief)
	{
		Txt_Thief->SetText(FText::FromString("Thief"));
		if (Txt_Cop)
			Txt_Cop->SetText(FText::GetEmpty());
	}
}
