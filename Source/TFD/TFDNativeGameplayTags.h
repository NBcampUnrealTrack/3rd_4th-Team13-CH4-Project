// TFDNativeGameplayTags.h
#pragma once
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

//TFD 프로젝트 용 태그 클래스

//1. 팀 구분 경찰/도둑/중립
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Team);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Team_Cop);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Team_Thief);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Team_Neutral);

//2.캐릭터 상태
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Character_State_Idle); //기본
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Character_State_Arrested); //체포됨
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Character_State_Hiding); // 은신
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Character_State_Stun); //기절
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Character_State_Detecting); //수색중
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Character_State_Release); //풀기 가능
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Character_State_StopMovement);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Character_State_RestartRandomMove);

//3.캐릭터 능력 
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Cop_Handcuff); //경찰-포획
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Thief_Steal); //도둑-훔치기
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Thief_Release); //도둑-풀어주기
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Thief_CancelRelease);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Thief_Invisibility); //도둑-은신
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Thief_Teleport); //도둑-텔레포트
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Neutral_RandomMove); //도둑-훔치기
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_FireProjectile); //도둑-훔치기
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_StopMovement);

//3-2 쿨타임관련
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Cop_Handcuff_CoolDown);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Thief_Steal_CoolDown);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Neutral_RandomMove_CoolDown); //도둑-훔치기
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_FireProjectile_CoolDown);
//4. object
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_Door); //문
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_Money); //돈
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SlowItem); // sample
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SlowItem_PickupItem);		// 스킬-겸용-줍는 아이템
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SlowItem_ThrowableItem);	// 스킬-겸용-던지는 아이템
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_Thief_PickupItem);	//스킬-도둑용-줍는 아이템 
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_Thief_ThrowableItem);	//스킬-도둑용-던지는 아이템
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_Cop_PickupItem);		//스킬-경찰용-줍는 아이템 
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_Cop_ThrowableItem);	//스킬-경찰용-던지는 아이템
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SKillStockItem_00);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SpawnArea); //스폰지역
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SpawnPoint); //스폰포인트
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SpawnPoint_Cop);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SpawnPoint_Thief);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SpawnPoint_Neutral);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SpawnPoint_Period);

//5.게임플레이큐 - 이펙트
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Character_Arrested); //체포 시 효과
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Character_HandCuff); //체포 시도 효과
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Character_Demerit); //체포 디메리트 효과
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Character_Release); // 풀어주기 시 효과

//6.GameplayEvent
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Cop_HandCuff_Apply); //잡기 발동 시점
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Cop_HandCuff_End);   //잡기 종료.

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Thief_Release_Apply);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Thief_Release_End);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Hiding);

//6.입력 - 스킬 슬롯
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Input_Skill_Slot1);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Input_Skill_Slot2);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Input_Skill_Slot3);



//7. 쿨타임 - 어빌리티 쿨타임
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Cooldown_Ability_Invisibility);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Cooldown_Ability_Teleport);


//8.GameAbility Tag
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameAbility);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameAbilityTask);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameAbilityTask_FireProjectile);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameAbilityEffect);
