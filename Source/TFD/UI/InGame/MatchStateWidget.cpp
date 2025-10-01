#include "MatchStateWidget.h"
#include "GameState/TFDGameState.h"
#include "GameInstance/TFDGameInstance.h"
#include "NativeGameplayTags.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Engine/AssetManager.h"

void UMatchStateWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;
	CachedGameState = PC->GetWorld()->GetGameState<ATFDGameState>();
	if (!CachedGameState) return;

	UE_LOG(LogTemp, Log, TEXT("[UPlayingWidget] CachedGameState found: %s"), *CachedGameState->GetName());
	// 델리게이트 바인딩
	CachedGameState->OnThiefScoreChanged.AddDynamic(this, &UMatchStateWidget::UpdateThiefScore);
	CachedGameState->OnThievesChanged.AddDynamic(this, &UMatchStateWidget::UpdateThiefCountAsync);
	CachedGameState->OnThiefArrayChanged.AddDynamic(this, &UMatchStateWidget::UpdateThiefArray);

	// 이미지 어레이 셋업
	UpdateThiefScore(0);
	InitCaughtJaiImages();
	UpdateThiefCountAsync();
}

void UMatchStateWidget::InitCaughtJaiImages()
{
	int32 CaughtThieves = CachedGameState->ThiefPlayerStateArray.Num();
	const int32 N = HBox_JailBG->GetChildrenCount();
	for (int32 i = 0; i < N; i++)
	{
		ESlateVisibility JailCntVisibility = ESlateVisibility::Hidden;
		if (i < CaughtThieves)
		{
			JailCntVisibility = ESlateVisibility::Visible;
		}
		HBox_JailBG->GetChildAt(i)->SetVisibility(JailCntVisibility);
		HBox_Jail->GetChildAt(i)->SetVisibility(JailCntVisibility);
	}

	JailCountImageArray.Reset();

	if (!HBox_Jail) return;

	//const int32 N = HBox_Jail->GetChildrenCount();
	JailCountImageArray.Reserve(N);

	for (int32 i = 0; i < N; ++i)
	{
		if (UImage* Img = Cast<UImage>(HBox_Jail->GetChildAt(i)))
		{
			JailCountImageArray.Add(Img);
		}
	}
}

void UMatchStateWidget::UpdateThiefScore(int32 NewScore)
{
	int32 TotalScoreGoal = CachedGameState->GetRuleData()->ThiefScoreForWin;
	ProgressBar_Gold->SetPercent(static_cast<float>(NewScore) / TotalScoreGoal);
	
	GoldText->SetText(FText::FromString(
		FString::Printf(TEXT("%d / %d"), NewScore, TotalScoreGoal))); 
}

void UMatchStateWidget::UpdateTeamIconAsync()
{
	ATFDPlayerState* PS = GetOwningPlayer()->GetPlayerState<ATFDPlayerState>();
	if (!PS)
	{
		// PlayerState가 아직 없으면 다음 Tick에 재시도
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			UpdateTeamIconAsync();
		});
		return;
	}

	FGameplayTag TeamTag = PS->GetActualTeam();

	if (!TeamTag.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			UpdateTeamIconAsync();
		});
	}

	FStreamableManager& SM = UAssetManager::GetStreamableManager();
	auto* GI = Cast<UTFDGameInstance>(GetGameInstance());

	auto UIResourceAsset = GI->UIResourceAsset; 
	if (UIResourceAsset.Get() == nullptr)
	{
		auto Path = UIResourceAsset.ToSoftObjectPath();
		SM.RequestAsyncLoad(Path,
			FStreamableDelegate::CreateWeakLambda(this, [this]()
		{
			UpdateTeamIconAsync();
		}));
	}

	auto CopTexture = UIResourceAsset->IconCops;
	auto ThiefTexture = UIResourceAsset->IconThief;
	
	TArray<FSoftObjectPath> Paths;
	Paths.Reserve(2);
	if (CopTexture.Get() == nullptr && CopTexture.ToSoftObjectPath().IsValid())
		Paths.Add(CopTexture.ToSoftObjectPath());
	if (ThiefTexture.Get() == nullptr && ThiefTexture.ToSoftObjectPath().IsValid())
		Paths.Add(ThiefTexture.ToSoftObjectPath());

	if (Paths.Num() > 0)
	{
		SM.RequestAsyncLoad(Paths,
			FStreamableDelegate::CreateWeakLambda(this, [this]()
		{
			UpdateThiefCountAsync();
		}));
		return;
	}
	UTexture2D* TeamTexture = TeamTag == TAG_Team_Cop ? CopTexture.Get() : ThiefTexture.Get();
	TeamIcon->SetBrushFromTexture(TeamTexture, true);
	
}

void UMatchStateWidget::UpdateThiefCountAsync()
{
	if (GetWorld() == nullptr || GetWorld()->GetGameState() == nullptr)
		return;

	auto* GS = Cast<ATFDGameState>(GetWorld()->GetGameState());

	int32 CaughtThieves = CachedGameState->CaughtThiefPlayerStateArray.Num();
	int32 ThievesNum = CachedGameState->ThiefPlayerStateArray.Num();
	
	for (int i = 0; i < ThievesNum; i++)
	{
		TObjectPtr<UImage> JailImg = JailCountImageArray[i];

		ESlateVisibility JailCntVisibility = ESlateVisibility::Hidden;
		if (i < CaughtThieves)
		{
			JailCntVisibility = ESlateVisibility::Visible;
		}
		JailImg->SetVisibility(JailCntVisibility);
	}
}
