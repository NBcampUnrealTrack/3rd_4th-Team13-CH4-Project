// UTFDSkillManagerComponent.cpp
#include "GameAbilitySystem/Component/TFDSkillManagerComponent.h"

UTFDSkillManagerComponent::UTFDSkillManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UTFDSkillManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UTFDSkillManagerComponent::InitializeASC()
{
}

void UTFDSkillManagerComponent::AddSkill(TSubclassOf<UGameplayAbility> SkillClass, FGameplayTag SkillTag, int32 InitialUsageCount)
{
}

void UTFDSkillManagerComponent::RemoveSkill(FGameplayTag SkillTag)
{
}

FTFDSkillSlot UTFDSkillManagerComponent::GetSkillAt(int32 SlotIndex) const
{
	return FTFDSkillSlot();
}

void UTFDSkillManagerComponent::UseSkill(FGameplayTag SkillTag)
{
}
