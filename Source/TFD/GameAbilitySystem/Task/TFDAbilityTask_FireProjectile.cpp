// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Task/TFDAbilityTask_FireProjectile.h"

#include "Character/TFDCharacterBase.h"
#include "GameAbilitySystem/Ability/TFDProjectileFireAbility.h"
#include "Kismet/GameplayStatics.h"

UTFDAbilityTask_FireProjectile* UTFDAbilityTask_FireProjectile::FireProjectile(UGameplayAbility* OwningAbility,
                                                                               const FTFDFireProjectileParams& Params)
{
	UTFDAbilityTask_FireProjectile* Task = NewAbilityTask<UTFDAbilityTask_FireProjectile>(OwningAbility);

	if (!Task)
	{
		return nullptr;
	}
	
	if (UTFDProjectileFireAbility* MyAbility = Cast<UTFDProjectileFireAbility>(OwningAbility))
	{
		UE_LOG(LogTemp, Warning, TEXT("FFF 엔드세팅"));
		Task->OnProjectileFired.AddDynamic(MyAbility, &UTFDProjectileFireAbility::OnProjectileFired);
	}
	Task->TaskParams = Params;
	Task->ReadyForActivation();



	return Task;
}

void UTFDAbilityTask_FireProjectile::Activate()
{
	Super::Activate();

	//GetCurrentActorInfo = GA가 실행될때 , FGameplayAbilityActorInfo라는 구조체를 만들어 주체에 대한 정보를 담아오는데 그거 가져오는 함수
	//IsNetAuthority = OwnerActor 기준으로  서버권한이 있는지 체크
	if (!Ability)
	{
		EndTask();
		return;
	}

	// 공통: 발사 이펙트(애니메이션, 사운드 등) → 클라/서버 둘 다 실행
	//우리게임은 투사체에서 처리함

	// 서버 권한에서만 실제 Projectile 생성
	if (Ability->GetCurrentActorInfo()->IsNetAuthority())
	{
		ShootProjectile(); // 실제 스폰 
	}
}

void UTFDAbilityTask_FireProjectile::EndTask()
{
	UE_LOG(LogTemp, Warning, TEXT("FFF 엔드테스크"));
	OnProjectileFired.Broadcast(); // Ability에게 알림
	Super::EndTask();
}

void UTFDAbilityTask_FireProjectile::ShootProjectile()
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	UAbilitySystemComponent* SourceASC = Ability->GetAbilitySystemComponentFromActorInfo();
	if (!CheckProjectile(*AvatarActor, *SourceASC))
	{
		EndTask();
		return;
	}

	// 스폰 준비: 액터를 월드에 바로 만들지 않고, 추가 설정을 할 수 있도록 메모리에만 준비.
	ATFDBaseObject* SpawnedProjectile = Cast<ATFDBaseObject>(
		UGameplayStatics::BeginDeferredActorSpawnFromClass(
			GetWorld(),
			TaskParams.ProjectileClass,
			SpawnWorldTransform,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
			AvatarActor
		)
	);

	if (SpawnedProjectile)
	{
		// ProjectileParams 적용
		SpawnedProjectile->SetBaseObjectParam(TaskParams.BaseObjectParam);
		// 발사체의 Instigator를 현재 Task의 실행 주체(사용자)로 설정
		SpawnedProjectile->SetInstigator(Cast<APawn>(AvatarActor));
		// 발사체 생명주기
		SpawnedProjectile->InitialLifeSpan = TaskParams.ProjectileLifeSpan;

		// // 컨텍스트(Context) 생성
		// FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
		// ContextHandle.AddInstigator(AvatarActor, SpawnedProjectile);
		// ContextHandle.AddSourceObject(AvatarActor);
		//
		// // Projectile 에게 전달
		// SpawnedProjectile->InitEffectContext(ContextHandle, TaskParams.DamageEffectClass);
		//
		
		// 스폰 완료: 준비된 액터를 월드에 최종적으로 배치.
		UGameplayStatics::FinishSpawningActor(SpawnedProjectile, SpawnWorldTransform);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" 발사체 스폰에 실패했습니다."));
	}

	EndTask();
}

bool UTFDAbilityTask_FireProjectile::CheckProjectile(AActor& pActor, UAbilitySystemComponent& SourceASC)
{
	bool result = false;

	if (!TaskParams.ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("발사체 블루프린트(ProjectileClass)가 지정되지 않았습니다."));

		EndTask();
		return result;
	}


	// AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!IsValid(&pActor))
	{
		UE_LOG(LogTemp, Error, TEXT("[AbilityTask_FireProjectile] : 아바타 액터가 유효하지 않습니다!"));
		// OnFinished.Broadcast();
		// EndTask();
		return result;
	}

	ATFDCharacterBase* Character = Cast<ATFDCharacterBase>(&pActor);

	if (!Character || !CalcSpawnTransform(Character, SpawnWorldTransform))
	{
		// EndTask();
		return result;
	}


	if (!IsValid(&SourceASC))
	{
		// EndTask();
		return result;
	}
	



	result = true;
	// EndTask();
	return result;
}

bool UTFDAbilityTask_FireProjectile::CalcSpawnTransform(ATFDCharacterBase* Character, FTransform& OutSpawnTransform)
{
	bool result = false;

	check(Character);
	USkeletalMeshComponent* pMesh = Character->GetMesh();
	check(pMesh);

	FTransform SocketWorldTransform;
	const FRotator CharacterRotation = Character->GetActorRotation();


	if (pMesh->DoesSocketExist(TaskParams.SocketName))
	{
		//SocketWorldTransform = pMesh->GetSocketTransform(Params.SocketName);
		const FVector SocketLocation = pMesh->GetSocketLocation(TaskParams.SocketName);


		SocketWorldTransform = FTransform(CharacterRotation, SocketLocation);
	}
	else
	{
		if (TaskParams.SocketName != NAME_None)
		{
			UE_LOG(LogTemp, Warning,
			       TEXT( "[DrgAbilityTask_FireProjectile] : 캐릭터 '%s'에서 소켓 '%s'를 찾을 수 없습니다. 캐릭터의 기본 위치에서 발사합니다." ),
			       *Character->GetName(), *TaskParams.SocketName.ToString());
		}

		// 소켓이 없을 경우 캐릭터의 위치와 회전을 기준으로
		// 이때 스케일이 커지는 문제를 방지하기 위해 위치와 회전만으로 새로 생성
		SocketWorldTransform = FTransform(CharacterRotation, Character->GetActorLocation());
	}

	OutSpawnTransform = TaskParams.ProjectileLocalTransform * SocketWorldTransform;

	result = true;
	
	return result;
}
