// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/MiniMapWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

#include "GameState/TFDGameState.h"
#include "PlayerState/TFDPlayerState.h"

#include "Object/JailCell.h"

#include "TFDNativeGameplayTags.h"

void UMiniMapWidget::NativeConstruct()
{
    Super::NativeConstruct();

    WorldMin = FVector2D(-6000.f, -7500.f);
    WorldMax = FVector2D(6000.f, 7500.f);

    if (MapImage)
    {
        //MapSize = MapImage->Brush.ImageSize;
        MapSize = MapImage->GetBrush().ImageSize;

        if (MapSize.IsZero())
        {
            if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(MapImage->Slot))
            {
                MapSize = CanvasSlot->GetSize();
            }
        }
    }

    CurrentGameState = GetWorld()->GetGameState<ATFDGameState>();
}

void UMiniMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!OwnerPawn || !PlayerIcon) return;

    FVector Location = OwnerPawn->GetActorLocation();

    float XRatio = (Location.Y - WorldMin.Y) / (WorldMax.Y - WorldMin.Y);
    float YRatio = (Location.X - WorldMin.X) / (WorldMax.X - WorldMin.X);

    FVector2D MiniMapPos = FVector2D(
        XRatio * MapSize.X,
        (1.f - YRatio) * MapSize.Y
    );

    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(PlayerIcon->Slot))
    {
        CanvasSlot->SetPosition(MiniMapPos);
    }

    // 팀원 아이콘 위치 갱신
    for (int32 i = 0; i < TeamPlayerIcons.Num(); i++)
    {
        if (!TeamPlayerIcons[i] || i >= TeamPlayerStateArray.Num()) continue;

        ATFDPlayerState* PS = TeamPlayerStateArray[i];
        if (!PS) continue;

        FVector PSLocation = PS->GetPawn() ? PS->GetPawn()->GetActorLocation() : FVector::ZeroVector;

        float X = (PSLocation.Y - WorldMin.Y) / (WorldMax.Y - WorldMin.Y) * MapSize.X;
        float Y = (1.f - (PSLocation.X - WorldMin.X) / (WorldMax.X - WorldMin.X)) * MapSize.Y;

        if (UCanvasPanelSlot* TeamSlot = Cast<UCanvasPanelSlot>(TeamPlayerIcons[i]->Slot))
        {
            TeamSlot->SetPosition(FVector2D(X, Y));
        }
    }
}

void UMiniMapWidget::SetOwnerPawn(APawn* InPawn)
{
    OwnerPawn = InPawn;
    TryInitializeOwnerPawnState();
}

void UMiniMapWidget::TryInitializeOwnerPawnState()
{
    if (!OwnerPawn) return;

    OwnerPawnState = OwnerPawn->GetPlayerState<ATFDPlayerState>();
    if (!OwnerPawnState)
    {
        // PawnState 아직 없으면 다음 Tick에 재시도
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UMiniMapWidget::TryInitializeOwnerPawnState);
        return;
    }

    if (!CurrentGameState) return;

    FGameplayTag TeamTag = OwnerPawnState->GetActualTeam();
    if (TeamTag == TAG_Team_Cop)
    {
        CurrentGameState->OnPoliceArrayChanged.AddDynamic(this, &UMiniMapWidget::UpdateTeamPlayerStateArray);
        CurrentGameState->OnPoliceItemArrayChanged.AddDynamic(this, &UMiniMapWidget::UpdateTeamItemIcon);
        UpdateTeamPlayerStateArray(CurrentGameState->PolicePlayerStateArray);
        UpdateTeamItemIcon(CurrentGameState->PoliceMapItemArray);
    }
    else if (TeamTag == TAG_Team_Thief)
    {
        CurrentGameState->OnThiefArrayChanged.AddDynamic(this, &UMiniMapWidget::UpdateTeamPlayerStateArray);
        CurrentGameState->OnThiefItemArrayChanged.AddDynamic(this, &UMiniMapWidget::UpdateTeamItemIcon);
        UpdateTeamPlayerStateArray(CurrentGameState->ThiefPlayerStateArray);
        UpdateTeamItemIcon(CurrentGameState->ThiefMapItemArray);
    }

    AJailCell* JailCell = CurrentGameState->GetWorldJailCell();
    if (!JailCell) return;

    FVector Location = JailCell->GetActorLocation();

    float XRatio = (Location.Y - WorldMin.Y) / (WorldMax.Y - WorldMin.Y);
    float YRatio = (Location.X - WorldMin.X) / (WorldMax.X - WorldMin.X);

    FVector2D MiniMapPos = FVector2D(
        XRatio * MapSize.X,
        (1.f - YRatio) * MapSize.Y
    );

    JailCellIcon->SetBrushFromTexture(JailIconTexture);

    if (UCanvasPanelSlot* JailCellSlot = Cast<UCanvasPanelSlot>(JailCellIcon->Slot))
    {
        JailCellSlot->SetPosition(MiniMapPos);
    }

}

void UMiniMapWidget::UpdateTeamPlayerStateArray(const TArray<TWeakObjectPtr<ATFDPlayerState>>& TeamArray)
{
    if (!OwnerPawnState) return;

    TeamPlayerStateArray.Empty();

    for (auto& WeakPS : TeamArray)
    {
        if (ATFDPlayerState* PS = WeakPS.Get())
        {
            if (PS != OwnerPawnState)
            {
                TeamPlayerStateArray.Add(PS);
            }
        }
    }

    // 기존 아이콘 제거
    for (UImage* Icon : TeamPlayerIcons)
    {
        if (Icon)
        {
            Icon->RemoveFromParent();
        }
    }
    TeamPlayerIcons.Empty();

    // 루트를 CanvasPanel로 캐스팅
    UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(GetRootWidget());
    if (!RootCanvas) return;

    // 팀원 아이콘 생성
    for (int32 i = 0; i < TeamPlayerStateArray.Num(); i++)
    {
        UImage* Icon = NewObject<UImage>(this);
        Icon->SetBrushFromTexture(TeamIconTexture);

        if (UCanvasPanelSlot* IconSlot = RootCanvas->AddChildToCanvas(Icon))
        {
            IconSlot->SetSize(FVector2D(16.f, 16.f));
            IconSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        }

        TeamPlayerIcons.Add(Icon);
    }

}

void UMiniMapWidget::UpdateTeamItemIcon(const TArray<TWeakObjectPtr<ATFDBaseObject>>& TeamItemArray)
{
    // 기존 아이콘 모두 제거
    for (UImage* Icon : TeamItemIcons)
    {
        if (Icon && Icon->IsValidLowLevel())
        {
            Icon->RemoveFromParent();
        }
    }
    TeamItemIcons.Empty();

    UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(GetRootWidget());
    if (!RootCanvas) return;

    // 아이템 배열 기준으로 새 아이콘 생성
    for (int32 i = 0; i < TeamItemArray.Num(); i++)
    {
        if (!TeamItemArray[i].IsValid()) continue;

        UImage* Icon = NewObject<UImage>(this);
        Icon->SetBrushFromTexture(TeamItemIconTexture);

        if (UCanvasPanelSlot* ItemIconSlot = RootCanvas->AddChildToCanvas(Icon))
        {
            ItemIconSlot->SetSize(FVector2D(16.f, 16.f));
            ItemIconSlot->SetAlignment(FVector2D(0.5f, 0.5f));

            // 월드 위치 가져오기
            FVector WorldPos = TeamItemArray[i]->GetActorLocation();

            // 미니맵 좌표 변환
            float X = (WorldPos.Y - WorldMin.Y) / (WorldMax.Y - WorldMin.Y) * MapSize.X;
            float Y = (1.f - (WorldPos.X - WorldMin.X) / (WorldMax.X - WorldMin.X)) * MapSize.Y;

            ItemIconSlot->SetPosition(FVector2D(X, Y));
        }

        TeamItemIcons.Add(Icon);
    }
}
