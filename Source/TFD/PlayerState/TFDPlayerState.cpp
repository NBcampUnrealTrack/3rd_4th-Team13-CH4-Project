// Fill out your copyright notice in the Description page of Project Settings.


#include "TFDPlayerState.h"

void ATFDPlayerState::SetTemaTag(FGameplayTag Tag)
{
	this->TeamTag = Tag;
}

FGameplayTag ATFDPlayerState::GetTemaTag() const
{
	return this->TeamTag;
}
