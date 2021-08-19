// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SimpleShooter/Enums/EPlayerMovementState.h"
#include "SimpleShooter/Enums/EPlayerFightState.h"
#include "BasePlayerAnimInstance.generated.h"

/**
 * 
 */
class AShooterCharacter;


UCLASS()
class SIMPLESHOOTER_API UBasePlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	
	UBasePlayerAnimInstance();
	UFUNCTION()
		virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	

public:
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		float AngleIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		float Angle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		float CashedAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		float SpeedPrevFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		float OffsetPitch;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		float OffsetYaw;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		float RightAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)//LeftHandIK
		float WeaponFABRIKAlpha;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		bool bIsJumping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		bool bIsCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		bool bIsAccelerating;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		bool bIsRightFootPlant;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		bool IsRightFootUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		EPlayerMovementState CurrentMovementState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		EPlayerMovementState PrevMovementState;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		EPlayerFightState CurrentFightState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		EPlayerFightState PrevFightState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		AShooterCharacter* PlayerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		FVector LastMovementInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		FVector PrevMovementInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		FTransform LeftHandIkTransform;
	

	float internalSpeed;


	UFUNCTION()
		void AnimNotify_WalkStartEntered();
	
	UFUNCTION()
		void AnimNotify_WalkEntered();
	
	UFUNCTION()
		void AnimNotify_WalkStartLeft();
	
	UFUNCTION()
		void AnimNotify_IdleEntered();

	UFUNCTION()
		void AnimNotify_WalkStopEntered();

	UFUNCTION()
		void AnimNotify_WalkStopLeft();


private:

	UPROPERTY(BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
		bool bWalkStartEntered;
	UPROPERTY(BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
		bool bWalkStopEntered;

	UPROPERTY(BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	FRotator ActorCachedRotation;

	UPROPERTY(BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
		FVector ActorCachedLocation;

	UPROPERTY(BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
		FVector ActorCachedForwardVector;

	UPROPERTY(BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
		FVector ActorCachedRightVector;

private:

	UFUNCTION()
		void TurnAndWalk();
	
	UFUNCTION()
		void CalculateLocationAndRotationMatching();

	UFUNCTION()
		void CalculateLocationMatching();

	UFUNCTION()
		void CalculateRotationMatching();

	UFUNCTION()
		void SetActorCachedDate();

	UFUNCTION()
		float CalculateAngleBetweenActorForwardAndMovementInput();

	UFUNCTION()
		void CalculateAndSetGunOrientation();

	UFUNCTION()
		FTransform CalculateLeftHandGunIk();

	UFUNCTION()
		void CalculateAndSetAimOffsetPitchAndYaw();




	UFUNCTION()
		void ShowAnimationDebug();


};
