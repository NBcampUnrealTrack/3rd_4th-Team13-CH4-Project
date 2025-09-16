#pragma once

UENUM(Blueprintable, meta = (DisplayName = "Hit Direction"))
enum class EHitDirection : uint8
{
	None    UMETA(DisplayName = "None"),
	Front   UMETA(DisplayName = "Front Hit"),
	Back    UMETA(DisplayName = "Back Hit"),
	Right   UMETA(DisplayName = "Right Hit"), 
	Left    UMETA(DisplayName = "Left Hit")
};