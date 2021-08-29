#pragma once

UENUM(BlueprintType)
enum class EPlayerCoverState : uint8
{
	None UMETA(DisplayName = "None"),
	CrouchCover UMETA(DisplayName = "CrouchCover"),
	StandingCover UMETA(DisplayName = "StandingCover"),
	RightPeekCover UMETA(DisplayName = "RightPeekCover"),
	LeftPeekCover UMETA(DisplayName = "RightPeekCover"),

};