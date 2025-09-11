// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUtility.h"

int UInGameUtility::GetPoliceRoleCount(int32 NumPlayers)
{
	int ResultPolice = 0;
	if (NumPlayers <= 1)
	{
		ResultPolice = 0;
		return 0;
	}

	if (NumPlayers == 2)
	{
		ResultPolice = 1;
	}
	else if (NumPlayers == 3)
	{
		ResultPolice = 1;
	}
	else // 4 ~ 6
	{
		ResultPolice = 2;
	}

	return ResultPolice;
}
