#pragma once
#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "MatchStateWidget.generated.h"

UCLASS()
class UMatchStateWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> TeamIcon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> GoldIcon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> JailIcon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TArray<UImage*> JailCountImageArray;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Gold;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UHorizontalBox* HBox_Jail;

	UFUNCTION(BlueprintCallable)
	void UpdateThiefScore(int32 NewScore);

	UFUNCTION(BlueprintCallable)
	void UpdateThiefCountAsync();

	UFUNCTION()
	void UpdateThiefArray(const TArray<TWeakObjectPtr<ATFDPlayerState>>& ThiefPlayerStateArray)
	{ UpdateThiefCountAsync();}
	
	UFUNCTION(BlueprintCallable)
	void UpdateTeamIconAsync();

	UFUNCTION(BlueprintCallable)
	void CollectJailImages();

private:
	class ATFDGameState* CachedGameState = nullptr;
};
