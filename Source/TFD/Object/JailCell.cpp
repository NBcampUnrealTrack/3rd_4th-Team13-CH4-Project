// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/JailCell.h"

#include "Components/BoxComponent.h"

#include "Controller/TFDPlayerController.h"
#include "Character/TFDCharacterBase.h"

#include "GameMode/TFDGameMode.h"
#include "GameState/TFDGameState.h"

#include "TFDNativeGameplayTags.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AJailCell::AJailCell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;          // << 이게 없으면 클라에는 복제 안됨

    JailCellBox = CreateDefaultSubobject<UBoxComponent>(TEXT("JailCellBox"));
    RootComponent = JailCellBox;
    JailCellBox->SetCollisionProfileName(TEXT("Trigger"));
    JailCellBox->OnComponentBeginOverlap.AddDynamic(this, &AJailCell::OnJailCellBoxOverlapBegin);
    JailCellBox->OnComponentEndOverlap.AddDynamic(this, &AJailCell::OnJailCellBoxOverlapEnd);

    OpenJailCellBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OpenJailCellBox"));
    OpenJailCellBox->SetupAttachment(RootComponent);
    OpenJailCellBox->SetCollisionProfileName(TEXT("Trigger"));
    OpenJailCellBox->OnComponentBeginOverlap.AddDynamic(this, &AJailCell::OnOpenJailCellBoxOverlapBegin);
    OpenJailCellBox->OnComponentEndOverlap.AddDynamic(this, &AJailCell::OnOpenJailCellBoxOverlapEnd);

    WallFront = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallFront"));
    WallFront->SetupAttachment(RootComponent);

    WallBack = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallBack"));
    WallBack->SetupAttachment(RootComponent);

    WallLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallLeft"));
    WallLeft->SetupAttachment(RootComponent);

    WallRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallRight"));
    WallRight->SetupAttachment(RootComponent);
}

void AJailCell::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AJailCell, CachedGameState);
    DOREPLIFETIME(AJailCell, InCharacters);
}

void AJailCell::EnterJail(ATFDCharacterBase* Character)
{
    if (!Character || !HasAuthority()) // 서버에서만 실행
        return;

    InCharacters.Add(Character);

    if (ATFDPlayerController* PC = Cast<ATFDPlayerController>(Character->GetController()))
    {
        PC->SetMovemnetWalking(true);
    }
}

void AJailCell::Release(ATFDCharacterBase* Character)
{
    if (!Character || !HasAuthority()) // 서버에서만 실행
        return;

    InCharacters.Remove(Character);

    if (Character->HasAuthority() && Character->GetAbilitySystemComponent() && CancleArrestedEffect)
    {
        FGameplayEffectContextHandle Context = Character->GetAbilitySystemComponent()->MakeEffectContext();
        Context.AddSourceObject(this);

        FGameplayEffectSpecHandle SpecHandle = Character->GetAbilitySystemComponent()->MakeOutgoingSpec(
            CancleArrestedEffect, 1, Context);

        if (SpecHandle.IsValid())
        {
            Character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(
                *SpecHandle.Data.Get(),
                Character->GetAbilitySystemComponent()
            );

            if (UWorld* World = GetWorld())
            {
                if (ATFDGameMode* GM = World->GetAuthGameMode<ATFDGameMode>())
                {
                    if (APawn* TargetPawn = Cast<APawn>(Character))
                    {
                        GM->OffCatchThief(TargetPawn);
                        UE_LOG(LogTemp, Warning, TEXT("OnCatchThief called for %s"), *TargetPawn->GetName());
                    }
                }
            }
            UE_LOG(LogTemp, Warning, TEXT("%s applied CancelArrestedEffect on Release"), *Character->GetName());
        }
    }
}

void AJailCell::HideWallsTemporarily()
{
    WallFront->SetVisibility(false, true);
    WallBack->SetVisibility(false, true);
    WallLeft->SetVisibility(false, true);
    WallRight->SetVisibility(false, true);

    // 콜리전도 비활성화
    WallFront->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WallBack->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WallLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WallRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    GetWorldTimerManager().SetTimer(
        ShowWallTimer,
        this,
        &AJailCell::ShowWalls,
        3.f,
        false
    );
}

void AJailCell::ShowWalls()
{
    // 벽 다시 보이기
    WallFront->SetVisibility(true, true);
    WallBack->SetVisibility(true, true);
    WallLeft->SetVisibility(true, true);
    WallRight->SetVisibility(true, true);

    // 콜리전 다시 활성화
    WallFront->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    WallBack->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    WallLeft->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    WallRight->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

// Called when the game starts or when spawned
void AJailCell::BeginPlay()
{
	Super::BeginPlay();
	
    if (HasAuthority())
    {
        CachedGameState = GetWorld()->GetGameState<ATFDGameState>();
        if (CachedGameState)
        {
            CachedGameState->RegisterJailCell(this);
            UE_LOG(LogTemp, Warning, TEXT("JailCell %s registered in GameState"), *GetName());
        }
    }
}

void AJailCell::OnJailCellBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority()) return;

    if (ATFDCharacterBase* Char = Cast<ATFDCharacterBase>(OtherActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("%s left JailCellBox"), *Char->GetName());
        EnterJail(Char);
    }
}

void AJailCell::OnJailCellBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!HasAuthority()) return;

    if (ATFDCharacterBase* Char = Cast<ATFDCharacterBase>(OtherActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("%s left JailCellBox"), *Char->GetName());
        Release(Char);
    }
}

void AJailCell::OnOpenJailCellBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ATFDCharacterBase* Char = Cast<ATFDCharacterBase>(OtherActor);
    if (!Char) return;

    if (InCharacters.Contains(Char)) return;


    ATFDPlayerController* PC = Cast<ATFDPlayerController>(Char->GetController());
    if (!PC) return;

    if (CachedGameState && CachedGameState->CaughtThiefPlayerStateArray.Num() <= 0) return;

    if (!ApplyReleaseAbilityEffect) return;
    
    UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent();
    if (!ASC) return;

    if (ASC->HasMatchingGameplayTag(TAG_Team_Cop) || ASC->HasMatchingGameplayTag(TAG_Character_State_Arrested)) return;

    if (PC->IsLocalController())
    {
        PC->HandleShowReleaseWidget();
    }
            

    FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
    Context.AddSourceObject(this);
            
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
        ApplyReleaseAbilityEffect, 1, Context);
            
    if (SpecHandle.IsValid())
    {
        ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        UE_LOG(LogTemp, Log, TEXT("%s: Release Ability Effect applied"), *Char->GetName());
    }
}

void AJailCell::OnOpenJailCellBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

    ATFDCharacterBase* Char = Cast<ATFDCharacterBase>(OtherActor);
    if (!Char) return;

    ATFDPlayerController* PC = Cast<ATFDPlayerController>(Char->GetController());
    if (!PC) return;

    if (PC->IsLocalController())
    {
        PC->HandleRemoveReleaseWidget();
    }
                
    UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent();
    if (!ASC) return;

    FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
    Context.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
        CancleReleaseAbilityEffect, 1, Context); // 레벨 1로 적용                

    if (SpecHandle.IsValid())
    {
        ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        UE_LOG(LogTemp, Log, TEXT("%s: Release Ability Effect Remove"), *Char->GetName());
    }
}