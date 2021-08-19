#pragma once
UENUM(BlueprintType)
enum class EPlayerMovementState : uint8
{
	Idle UMETA(DisplayeName = "Idle"),
	Walking UMETA(DisplayeName = "Walking"),
	Running UMETA(DisplayeName = "Running"),
	Jumping UMETA(DisplayeName = "Jumping")
};