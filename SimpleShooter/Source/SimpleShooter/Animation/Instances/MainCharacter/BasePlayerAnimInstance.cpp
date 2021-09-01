// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerAnimInstance.h"
#include "SimpleShooter/Enums/EPlayerMovementState.h"
#include "SimpleShooter/Enums/EPlayerFightState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SimpleShooter/Player/Character/ShooterCharacter.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "SimpleShooter/Guns/Gun.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
UBasePlayerAnimInstance::UBasePlayerAnimInstance()
{
	CashedAngle = 0;
	Speed = 0;
	internalSpeed = 0;
	WeaponFABRIKAlpha = 1;
	bIsLeavingCoverTurn = false;
}

void UBasePlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	APawn* PlayerPawn = TryGetPawnOwner();

	if (!PlayerPawn)
	{
		return;
	}

	PlayerCharacter = Cast<AShooterCharacter>(PlayerPawn);

	if(PlayerCharacter)
	{
		SpeedPrevFrame = Speed;
		Speed = PlayerCharacter->GetVelocity().Size();
		bIsDead = PlayerCharacter->IsDead();

		

		if(CurrentMovementState != PlayerCharacter->GetMovementState() )
		{
			PrevMovementState = CurrentMovementState;
			CurrentMovementState = PlayerCharacter->GetMovementState();
		}

		PrevFightState = CurrentFightState;
		CurrentFightState = PlayerCharacter->GetFightState();
		bIsAccelerating = PlayerCharacter->IsAccelerating();
		RightAxis = PlayerCharacter->GetRightAxis();

		CurrentCoverState = PlayerCharacter->GetCoverState();

		PrevMovementInput = LastMovementInput;
		LastMovementInput = PlayerCharacter->GetLastMovementInputVector().GetSafeNormal();
		bIsRightMoveEnabled = PlayerCharacter->GetIsRightMoveEnable();
		bIsForwardMoveEnabled = PlayerCharacter->GetIsForwardMoveEnable();


		CashedAnglePrevFrame = CashedAngle;
		CashedAngle = CalculateAngleBetweenActorForwardAndMovementInput();
		if(bWalkStartEntered)
		{
			//Previously i was using curve but now root motion is used.
			
			//CalculateLocationAndRotationMatching();
			//CalculateRotationMatching();
			
		}
		else
		{
			
			Angle = CalculateAngleBetweenActorForwardAndMovementInput();
		}


		if(bWalkStopEntered)
		{
			//CalculateLocationMatching();
		}


		if(PlayerCharacter->HasWeaponInHand() && !PlayerCharacter->IsReloading())
		{

			CalculateAndSetGunOrientation();
			LeftHandIkTransform = CalculateLeftHandGunIk();
			CalculateAndSetAimOffsetPitchAndYaw();
			
		}
		else
		{
			WeaponFABRIKAlpha = 0;
		}


		if(PlayerCharacter->GetAnimationDebug())
		{

			ShowAnimationDebug();

		}
		
		
	}

}

void UBasePlayerAnimInstance::AnimNotify_WalkStartEntered()
{
	if (PlayerCharacter)
	{
		if(PlayerCharacter->GetFightState() != EPlayerFightState::Aim)
		{
			PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		else
		{
			
		}
		
		SetActorCachedDate();
		bWalkStartEntered = true;
	}

}

void UBasePlayerAnimInstance::AnimNotify_WalkEntered()
{
	if (PlayerCharacter)
	{
		//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1); //Global slow motion.
		SetActorCachedDate();
		if (PlayerCharacter->GetFightState() == EPlayerFightState::Aim)
		{
			PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		else
		{
			PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		}
		bWalkStartEntered = false;
	}
}

void UBasePlayerAnimInstance::AnimNotify_WalkStartLeft()
{
	
	if(PlayerCharacter)
	{
		//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1); //Global slow motion.
		SetActorCachedDate();
		if (PlayerCharacter->GetFightState() == EPlayerFightState::Aim)
		{
			PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		else
		{
			PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		}
			bWalkStartEntered = false;
	}

}

void UBasePlayerAnimInstance::AnimNotify_IdleEntered()
{
	if(PlayerCharacter)
	{
		SetActorCachedDate();
		PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	
}

void UBasePlayerAnimInstance::AnimNotify_WalkStopEntered()
{
	
	if(PlayerCharacter)
	{
		if (PlayerCharacter->GetFightState() != EPlayerFightState::Aim)
		{
			PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		else
		{

		}
		

		
		SetActorCachedDate();
		bWalkStopEntered = true;
	}
}

void UBasePlayerAnimInstance::AnimNotify_WalkStopLeft()
{
	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetFightState() != EPlayerFightState::Aim)
		{

			PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		else
		{

		}
		
		//PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		SetActorCachedDate();
		bWalkStopEntered = false;
	}

	
	
}

void UBasePlayerAnimInstance::AnimNotify_CoverTurnEntered()
{
	bIsLeavingCoverTurn = false;
	bCharacterIsFacingRightCached = bCharacterIsFacingRight;
	if(PlayerCharacter)
	{
		PlayerCharacter->GetMovementComponent()->StopMovementImmediately();
	}
}

void UBasePlayerAnimInstance::AnimNotify_CoverTurnLeft()
{
	bIsLeavingCoverTurn = true;
	bCharacterIsFacingRightCached = bCharacterIsFacingRight;
}

bool UBasePlayerAnimInstance::IsCharacterFacingRight()
{
	return bCharacterIsFacingRight;
}


void UBasePlayerAnimInstance::TurnAndWalk()
{
	bool IsTurning = false;
	if(!IsTurning)
	{
		//CashedAngle = Angle;
	}

	float OutCurve;
	if (this->GetCurveValue("RotationCurveAlpha", OutCurve))
	{

		
		
		PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		FRotator PlayerNewRotation =  FMath::Lerp(PlayerCharacter->GetActorRotation(),
					PlayerCharacter->GetActorRotation() + FRotator(0, CashedAngle, 0),
					OutCurve);

		PlayerCharacter->SetActorRotation(PlayerNewRotation);
		
		IsTurning = true;
	}
	else
	{
		//PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		IsTurning = false;
	}
	
	
}

void UBasePlayerAnimInstance::CalculateLocationAndRotationMatching()
{

	CalculateLocationMatching();
	CalculateRotationMatching();

}

void UBasePlayerAnimInstance::CalculateLocationMatching()
{

	float YValue = 0;
	float XValue = 0;

	if (this->GetCurveValue("LocationCurveY", YValue) && this->GetCurveValue("LocationCurveX", XValue))
	{

		
		//UE_LOG(LogTemp, Warning, TEXT("LocationMatching: %s"), *(PlayerCharacter->GetActorLocation().ToString()))
			PlayerCharacter->SetActorLocation(ActorCachedLocation + ActorCachedForwardVector * YValue + ActorCachedRightVector * -1 * XValue,true);
	}

}

void UBasePlayerAnimInstance::CalculateRotationMatching()
{
	
	float YawValue = 0;

	if (this->GetCurveValue("RotationCurve", YawValue))
	{
		//UE_LOG(LogTemp, Warning, TEXT("RotationMatching: %s :  %f"), *(PlayerCharacter->GetActorRotation().ToString()), YawValue)
			PlayerCharacter->SetActorRotation(ActorCachedRotation + FRotator(0, YawValue, 0));

	}
}

void UBasePlayerAnimInstance::SetActorCachedDate()
{
	
	ActorCachedRotation = PlayerCharacter->GetActorRotation();
	ActorCachedLocation = PlayerCharacter->GetActorLocation();

	ActorCachedForwardVector = PlayerCharacter->GetActorForwardVector();
	ActorCachedRightVector = PlayerCharacter->GetActorRightVector();
}

float UBasePlayerAnimInstance::CalculateAngleBetweenActorForwardAndMovementInput()
{
	if(PlayerCharacter)
	{
		/*const FVector ActorForwardVectorNormalize = PlayerCharacter->GetActorForwardVector().GetSafeNormal();
		const FVector LastMovementInputVectorNormalize = PlayerCharacter->GetLastMovementInput();
		const float ForwardVecDotInputVec = FVector::DotProduct(LastMovementInputVectorNormalize, ActorForwardVectorNormalize);
		const FVector ForwardVecCrossInputVec = FVector::CrossProduct(LastMovementInputVectorNormalize, ActorForwardVectorNormalize);
		const int Neg = (ForwardVecCrossInputVec.Z < 0 ? 1 : -1);


		return Neg * UKismetMathLibrary::DegAcos(ForwardVecDotInputVec);*/

		/*const int ActorForwardYaw = int((PlayerCharacter->GetActorForwardVector().Rotation().Yaw + 360)) % 360;
		const int ControllerYaw = PlayerCharacter->GetControlRotation().Yaw;
		return ControllerYaw - ActorForwardYaw;*/


		/*const FVector ActorForwardVectorNormalize = PlayerCharacter->GetActorForwardVector();
		UE_LOG(LogTemp, Warning, TEXT("GetActorForwardVector rotation: %s"), *(PlayerCharacter->GetActorForwardVector().Rotation().ToString()));
		const FVector LastMovementInputVectorNormalize = (PlayerCharacter->GetControlRotation() + PlayerCharacter->GetLastMovementInput().Rotation()).Vector();
		UE_LOG(LogTemp, Warning, TEXT("Control rotation: %s"), *(PlayerCharacter->GetControlRotation().ToString()));
		UE_LOG(LogTemp, Warning, TEXT("GetLastMovementInput Rotation: %s"), *(PlayerCharacter->GetLastMovementInput().Rotation().ToString()));
		const float ForwardVecDotInputVec = FVector::DotProduct(LastMovementInputVectorNormalize, ActorForwardVectorNormalize);
		const FVector ForwardVecCrossInputVec = FVector::CrossProduct(LastMovementInputVectorNormalize, ActorForwardVectorNormalize);
		const int Neg = (ForwardVecCrossInputVec.Z < 0 ? 1 : -1);


		return Neg * UKismetMathLibrary::DegAcos(ForwardVecDotInputVec);*/

		int ForwardYaw = (PlayerCharacter->GetActorForwardVector().Rotation().Yaw ) ;
		UE_LOG(LogTemp, Warning, TEXT("GetActorForwardVector rotation: %s"), *(PlayerCharacter->GetActorForwardVector().Rotation().ToString()));
		float ControllerYaw = PlayerCharacter->GetControlRotation().Yaw;
		ControllerYaw = ControllerYaw < 180 ? ControllerYaw : ControllerYaw - 360;
		
		
		const float ControllerInputYaw = ControllerYaw + PlayerCharacter->GetLastMovementInput().Rotation().Yaw;
		UE_LOG(LogTemp, Warning, TEXT("Control rotation: %s"), *(PlayerCharacter->GetControlRotation().ToString()));
		UE_LOG(LogTemp, Warning, TEXT("GetLastMovementInput Rotation: %s"), *(PlayerCharacter->GetLastMovementInput().Rotation().ToString()));

		float delta = ControllerInputYaw - ForwardYaw;
	
		if(delta  > 0 && delta > 180)
		{
			delta -= 360;
		}
		else if (delta <0 && delta < -180)
		{
			delta += 360;
		}
		
		return delta;

	}
	return 0;
}

void UBasePlayerAnimInstance::CalculateAndSetGunOrientation()
{
	

	
	const FVector WeaponSocketLocation = PlayerCharacter->GetMesh()->GetSocketLocation("WeaponSocket");
	FVector MidlleFingerLocation = PlayerCharacter->GetMesh()->GetBoneLocation("middle_01_l");
	MidlleFingerLocation.Z -= 2.f;
	const FVector WeaponDirection = (MidlleFingerLocation - WeaponSocketLocation);

	//setting gun location and rotation
	PlayerCharacter->GetCurrentWeaponRefrence()->SetActorRotation(WeaponDirection.Rotation());
	PlayerCharacter->GetCurrentWeaponRefrence()->SetActorLocation(PlayerCharacter->GetMesh()->GetSocketLocation("WeaponSocket"));
	/*
	DrawDebugSphere(GetWorld(), PlayerCharacter->GetCurrentWeaponRefrence()->GetActorLocation(), 4.f, 4, FColor::Red, false, 0.1);
	DrawDebugSphere(GetWorld(), PlayerCharacter->GetMesh()->GetSocketLocation("WeaponSocket"), 10.f, 4, FColor::Blue, false, 0.1);*/

}

FTransform UBasePlayerAnimInstance::CalculateLeftHandGunIk()
{
	FTransform LeftHandIK;
	USkeletalMeshComponent* PlayerGunMesh = PlayerCharacter->GetCurrentWeaponRefrence()->GetMesh();
	
	const FVector GunLeftHandIKSocketLocation = PlayerGunMesh->GetSocketTransform("LeftHandIK").GetLocation();

	FVector OutBoneSpacePositionRelatedToRightHand;
	FRotator OutBoneSpaceRotationRelatedToRightHand;
	PlayerCharacter->GetMesh()->TransformToBoneSpace("Hand_R", GunLeftHandIKSocketLocation, FRotator(),
		OutBoneSpacePositionRelatedToRightHand, OutBoneSpaceRotationRelatedToRightHand);

	DrawDebugSphere(GetWorld(), GunLeftHandIKSocketLocation, 5.f, 5, FColor::Blue, false, 0.1);


	LeftHandIK.SetLocation(OutBoneSpacePositionRelatedToRightHand);
	LeftHandIK.SetRotation(FQuat(OutBoneSpaceRotationRelatedToRightHand));
	WeaponFABRIKAlpha = 1;

	return LeftHandIK;
}

void UBasePlayerAnimInstance::CalculateAndSetAimOffsetPitchAndYaw()
{

	if(PlayerCharacter)
	{
		const FRotator PlayerControlRotation = PlayerCharacter->GetControlRotation();
		const FRotator PlayerRotation = PlayerCharacter->GetActorRotation();

		const FRotator TargetDeltaRotation = PlayerControlRotation - PlayerRotation;
		const FRotator Current = FRotator(OffsetPitch, OffsetYaw, 0);
		const FRotator InterpRotation = FMath::RInterpTo(Current, TargetDeltaRotation, GetWorld()->GetDeltaSeconds(), 15.f);
		OffsetPitch = FMath::ClampAngle( InterpRotation.Pitch,-90,90);
		OffsetYaw = FMath::ClampAngle(InterpRotation.Yaw, -90, 90);
	}
}

void UBasePlayerAnimInstance::SetMovementEnable(bool bEnable)
{
	if(PlayerCharacter)
	{
		PlayerCharacter->SetMovementEnable(bEnable);
	}
}

void UBasePlayerAnimInstance::ShowAnimationDebug()
{
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Orange, "Angle: " + FString::FromInt(Angle));
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Orange, "CashedAngle:  " + FString::FromInt(CashedAngle));
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Orange, "CashedAnglePrevFrame:  " + FString::FromInt(CashedAnglePrevFrame));

	const FString CurrentMovementEnumString = UEnum::GetValueAsString<EPlayerMovementState>(CurrentMovementState);
	const FString PrevMovementEnumString = UEnum::GetValueAsString<EPlayerMovementState>(PrevMovementState);
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Orange, "CurrentMovementState:  " + CurrentMovementEnumString);
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Orange, "PrevMovementState:  " + PrevMovementEnumString);
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Orange, "PitchOffset:  " + FString::SanitizeFloat(OffsetPitch));
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Orange, "YawOffset:  " + FString::SanitizeFloat(OffsetYaw));
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Orange, "Speed:  " + FString::SanitizeFloat(Speed));
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Orange, "CharacterIsFacingRight:  " + FString(bCharacterIsFacingRight ? "True" : "False"));
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Orange, "CharacterIsFacingRight Cached:  " + FString(bCharacterIsFacingRightCached ? "True" : "False"));
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Orange, "RightAxis:  " + FString::SanitizeFloat(RightAxis));

}


