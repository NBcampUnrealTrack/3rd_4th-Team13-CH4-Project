// TFDNativeGameplayTags.cpp
#include "TFDNativeGameplayTags.h"

//1. 팀 구분 경찰/도둑/중립

UE_DEFINE_GAMEPLAY_TAG(TAG_Team,"Team");
UE_DEFINE_GAMEPLAY_TAG(TAG_Team_Cop, "Team.Cop");
UE_DEFINE_GAMEPLAY_TAG(TAG_Team_Thief, "Team.Thief");
UE_DEFINE_GAMEPLAY_TAG(TAG_Team_Neutral, "Team.Neutral");

//2.캐릭터 상태
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Arrested, "Character.State.Arrested");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Hiding, "Character.State.Hiding");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Idle, "Character.State.Idle");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Stun, "Character.State.Stun");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Detecting, "Character.State.Detecting");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_Release, "Character.State.Release");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_StopMovement, "Character.State.StopMovement");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_State_RestartRandomMove, "Character.State.RestartRandomMove");
//3.캐릭터 능력 
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Cop_Handcuff, "Ability.Cop.Handcuff");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Thief_Steal, "Ability.Thief.Steal");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Thief_Release, "Ability.Thief.Release");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Thief_CancelRelease, "Ability.Thief.CancelRelease");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Thief_Invisibility, "Ability.Thief.Invisibility");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Thief_Teleport, "Ability.Thief.Teleport");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Neutral_RandomMove, "Ability.Neutral.RandomMove");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_StopMovement, "Ability.StopMovement");


UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_FireProjectile,"Ability.FireProjectile"); //도둑-훔치기
//3-2 쿨타임관련
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Cop_Handcuff_CoolDown,"Ability.Cop.Handcuff.CoolDown"); //도둑-훔치기
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Thief_Steal_CoolDown,"Ability.Thief.Steal.CoolDown"); //도둑-훔치기
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Neutral_RandomMove_CoolDown, "Ability.Neutral.RandomMove.CoolDown");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_FireProjectile_CoolDown,"Ability.FireProjectile.CoolDown"); //도둑-훔치기

//4. object
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_Door, "Object.Door"); //문
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_Money, "Object.Money"); //돈
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_SpawnArea, "Object.SpawnArea"); //스폰
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_SlowItem, "Object.SlowItem"); //스폰
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_SlowItem_PickupItem, "Object.SlowItem.PickupItem");		// 스킬-겸용-줍는 아이템
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_SlowItem_ThrowableItem, "Object.SlowItem.ThrowableItem");	// 스킬-겸용-던지는 아이템
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_Thief_PickupItem, "Object.Thief.PickupItem");			//스킬-도둑용-줍는 아이템 
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_Thief_ThrowableItem, "Object.Thief.ThrowableItem");	//스킬-도둑용-던지는 아이템
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_Cop_PickupItem, "Object.Cop.PickupItem");				//스킬-경찰용-줍는 아이템 
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_Cop_ThrowableItem, "Object.Cop.ThrowableItem");		//스킬-경찰용-던지는 아이템
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_SKillStockItem_00, "Object.SkillStockItem_00"); //스폰
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_SpawnPoint, "Object.SpawnPoint"); //스폰포인트
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_SpawnPoint_Cop, "Object.SpawnPoint.Cop"); //스폰포인트
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_SpawnPoint_Thief, "Object.SpawnPoint.Thief"); //스폰포인트
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_SpawnPoint_Neutral, "Object.SpawnPoint.Neutral"); //스폰포인트
UE_DEFINE_GAMEPLAY_TAG(TAG_Object_SpawnPoint_Period, "Object.SpawnPoint.Period"); //스폰포인트


//5.게임플레이큐 - 이펙트
UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_Character_Arrested, "GameplayCue.Character.Arrested");
UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_Character_HandCuff, "GameplayCue.Character.HandCuff");
UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_Character_Demerit, "GameplayCue.Character.Demerit");
UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_Hiding, "GameplayCue.Hiding");

//GameplayEvent
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Cop_HandCuff_Apply, "Ability.Cop.HandCuff.Apply"); // 체포 발동 시점 GameplayEvent 수신 태그
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Cop_HandCuff_End, "Ability.Cop.HandCuff.End"); // 체포 발동 시점 GameplayEvent 수신 태그

UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Thief_Release_Apply, "Ability.Thief.Release.Apply");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Thief_Release_End, "Ability.Thief.Release.End");
// 6. 입력 - 스킬 슬롯
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Skill_Slot1, "Input.Skill.Slot1");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Skill_Slot2, "Input.Skill.Slot2");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Skill_Slot3, "Input.Skill.Slot3");

//7. 쿨타임 - 어빌리티 쿨타임
UE_DEFINE_GAMEPLAY_TAG(TAG_Cooldown_Ability_Invisibility, "Cooldown.Ability.Invisibility");
UE_DEFINE_GAMEPLAY_TAG(TAG_Cooldown_Ability_Teleport, "Cooldown.Ability.Teleport");



//8.GameAbility Tag
UE_DEFINE_GAMEPLAY_TAG(TAG_GameAbility, "GameAbility");
UE_DEFINE_GAMEPLAY_TAG(TAG_GameAbilityTask, "GameAbilityTask");
UE_DEFINE_GAMEPLAY_TAG(TAG_GameAbilityTask_FireProjectile, "GameAbilityTask.FireProjectile");
UE_DEFINE_GAMEPLAY_TAG(TAG_GameAbilityEffect, "GameAbilityEffect");

