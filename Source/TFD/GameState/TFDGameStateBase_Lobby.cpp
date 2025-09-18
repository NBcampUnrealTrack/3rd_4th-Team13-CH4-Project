// ATFDGameStateBase_Lobby.cpp
#include "GameState/TFDGameStateBase_Lobby.h"

#include "GameFramework/PlayerState.h"
#include "Controller/TFDPlayerController.h"
#include "Net/UnrealNetwork.h"

ATFDGameStateBase_Lobby::ATFDGameStateBase_Lobby()
{
	// 복제 활성화
	bReplicates = true;
}

int32 ATFDGameStateBase_Lobby::GetConnectedPlayerCount() const
{
	return PlayerArray.Num();
}

void ATFDGameStateBase_Lobby::HandleBeginPlay()
{
	Super::HandleBeginPlay();

	// 처음 플레이 시작 시 접속자 리스트 한 번 갱신
	OnPlayerListChanged.Broadcast();
}

void ATFDGameStateBase_Lobby::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	FString PlayerName = PlayerState ? PlayerState->GetPlayerName() : TEXT("nullptr");
	UE_LOG(LogTemp, Error, TEXT("[ATFDGameStateBase_Lobby][AddPlayerState] Player joined: %s"), *PlayerName);

	// 접속자 리스트 갱신
	// 지연 브로드캐스트로 변경 (동시 접속 시 UI가 한 번에 갱신되도록)
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{OnPlayerListChanged.Broadcast(); }, 0.2f, false);
}
void ATFDGameStateBase_Lobby::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	UE_LOG(LogTemp, Log, TEXT("[ATFDGameStateBase_Lobby][RemovePlayerState] Player left: %s"), *PlayerState->GetPlayerName());
	OnPlayerListChanged.Broadcast(); // 접속자 리스트 갱신
}

void ATFDGameStateBase_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATFDGameStateBase_Lobby, bLobbyStarted);

}

void ATFDGameStateBase_Lobby::StartLobby()
{
    if (HasAuthority())
    {
        bLobbyStarted = true;
        OnRep_LobbyStarted(); // 서버에서도 즉시 처리
    }
}

void ATFDGameStateBase_Lobby::OnRep_LobbyStarted()
{
    if (bLobbyStarted)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnRep_LobbyStarted triggered on %s"), *GetName());

        // 로컬 컨트롤러만 UI 실행
        if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
        {
            if (ATFDPlayerController* TFDPC = Cast<ATFDPlayerController>(PC))
            {
                if (TFDPC->IsLocalController())
                {
                    TFDPC->EnterLobby();
                }
            }
        }
    }
}