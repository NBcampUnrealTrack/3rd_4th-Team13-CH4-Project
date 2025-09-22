// TFDNativeGameplayTags.h
#pragma once
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

//TFD 프로젝트 용 태그 클래스

//1. 팀 구분 경찰/도둑/중립
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

//3.캐릭터 능력 
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Cop_Handcuff); //경찰-포획
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Thief_Steal); //도둑-훔치기
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Thief_Release); //도둑-풀어주기
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Thief_Invisibility); //도둑-은신
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Neutral_RandomMove); //도둑-훔치기

//4. object
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_Door); //문
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_Money); //돈
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SlowItem);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SKillStockItem_00);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SpawnArea); //스폰지역
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SpawnPoint); //스폰포인트
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SpawnPoint_Cop); 
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SpawnPoint_Thief); 
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SpawnPoint_Neutral); 
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Object_SpawnPoint_Period); 

//5.게임플레이큐 - 이펙트
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Arrested); //체포 시 효과
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Hiding);

//6.입력 - 스킬 슬롯
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Input_Skill_Slot1);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Input_Skill_Slot2);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Input_Skill_Slot3);

//7. 쿨타임 - 어빌리티 쿨타임
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Cooldown_Ability_Invisibility);