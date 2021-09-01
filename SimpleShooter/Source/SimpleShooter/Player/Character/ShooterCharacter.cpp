// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include <ThirdParty/CryptoPP/5.6.5/include/misc.h>

#include "SimpleShooter/Guns/Gun.h"
#include "Components/CapsuleComponent.h"
#include "SimpleShooter/GameModes/SimpleShooterGameModeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "Engine.h"
#include "Engine/EngineBaseTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerController.h"
#include "SimpleShooter/Animation/Instances/MainCharacter/BasePlayerAnimInstance.h"


// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Initialization

	
	
	CurrentPlayerMovementState = EPlayerMovementState::Idle;
	CurrentPlayerFightState = EPlayerFightState::NotArmed;
	CurrentPlayerCoverState = EPlayerCoverState::None;

	MaxWalkSpeed = 300;
	MaxRunSpeed = 600;
	MaxCrouchSpeed = 170;
	FOVSpeed = 1;
	NotArmedSpeedCoefficient = 0;
	ArmedSpeedCoefficient = 50;
	AimSpeedCoefficient = 150;
	SpeedDecelerator = NotArmedSpeedCoefficient;
	CurrentGunIndex = -1;
	bIsHoldingAimButton = false;
	NormalAcceleration = 2048;
	CrouchAcceleration = 100;

	CurrentSpeedCoefficient = NotArmedSpeedCoefficient;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed - SpeedDecelerator;
	GetCharacterMovement()->MaxWalkSpeedCrouched = MaxCrouchSpeed - SpeedDecelerator;

	bIsRightMoveEnabled = true;
	bIsForwardMoveEnabled = true;

	
	bAnimationDebug = false;
	bCharacterDebug = true;
	bGunDebug = false;
	
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	UE_LOG(LogTemp,Warning , TEXT("%d"), Health);

	CoverComponent = FindComponentByClass<UCoverComponent>();
	if (!CoverComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("No Cover Component Found"));
	}

	

}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	///GetCharacterMovement()->bOrientRotationToMovement = false;
	///GetCharacterMovement()->bUseControllerDesiredRotation = false;
	///bUseControllerRotationYaw = false;


	
	if(bWantsToGrabWeapon)
	{
		GoToNearestWeaponLocation();
	}



	//Debug massages
	if(bCharacterDebug)
	{
		CharacterDebug();
	}
	
	if(bGunDebug)
	{
		if (GunInventory.Num() != 0)
		{
			if (GunInventory[CurrentGunIndex])
			{
				GunInventory[CurrentGunIndex]->Printstatistics(DeltaTime);
			}
			
		}
	}
	

}



// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::Moveforward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &AShooterCharacter::LookRightRate);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AShooterCharacter::LookRightRate);


	PlayerInputComponent->BindAction(TEXT("Jump"),  EInputEvent::IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AShooterCharacter::ShootStart);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Released, this, &AShooterCharacter::ShootEnd);
	PlayerInputComponent->BindAction(TEXT("Sprint"),EInputEvent::IE_Pressed, this , &AShooterCharacter::RunStart);
	PlayerInputComponent->BindAction(TEXT("Sprint"),EInputEvent::IE_Released, this , &AShooterCharacter::RunEnd);
	PlayerInputComponent->BindAction(TEXT("EquipWeapon"), EInputEvent::IE_Pressed , this , &AShooterCharacter::EquipWeaponInput);
	
	PlayerInputComponent->BindAction(TEXT("DropWeapon"), EInputEvent::IE_Pressed , this , &AShooterCharacter::DropWeapon);
	PlayerInputComponent->BindAction(TEXT("ChangeGunInInventory"), EInputEvent::IE_Pressed , this , &AShooterCharacter::ChangeGunInInventory);

	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this , &AShooterCharacter::Reload);
	PlayerInputComponent->BindAction(TEXT("Aim"), EInputEvent::IE_Pressed , this, &AShooterCharacter::AimStart);
	PlayerInputComponent->BindAction(TEXT("Aim"), EInputEvent::IE_Released , this, &AShooterCharacter::AimEnd);
	// add crouch
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &AShooterCharacter::CrouchAndCoverStart);
	//PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Released, this, &AShooterCharacter::CrouchAndCoverEnd);

	//Debug
	PlayerInputComponent->BindAction(TEXT("AnimationDebug"), EInputEvent::IE_Pressed, this, &AShooterCharacter::FlipFlopAnimationDebug);
	PlayerInputComponent->BindAction(TEXT("CharacterDebug"), EInputEvent::IE_Pressed, this, &AShooterCharacter::FlipFlopCharacterDebug);
	PlayerInputComponent->BindAction(TEXT("GunDebug"), EInputEvent::IE_Pressed, this, &AShooterCharacter::FlipFlopGunDebug);

	
}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

	if (IsDead()) return 0.0f;

	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("%s received damage! %f health remaining"), *GetOwner()->GetName(), Health);

	if (IsDead())
	{
		if (GetController())
		{
			ASimpleShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASimpleShooterGameModeBase>();
			if (GameMode)
			{
				GameMode->PawnKilled(this);
			}
			DetachFromControllerPendingDestroy();
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		}
			
	}

	return DamageToApply;
}

bool AShooterCharacter::IsDead() const
{
	return Health <= 0;
}

bool AShooterCharacter::IsAccelerating() const
{
	return  !(FMath::IsNearlyEqual(GetCharacterMovement()->GetCurrentAcceleration().Size() , 0.0f , 0.1f));
}

float AShooterCharacter::GetHealthPercentage() const
{
	return Health / MaxHealth;
}

bool AShooterCharacter::GetAnimationDebug() const
{
	return bAnimationDebug;
}

void AShooterCharacter::Moveforward(float AxisValue)
{
	LastMovementInput.X = AxisValue;

	if(!bIsForwardMoveEnabled)
	{
		return;
	}

	
	//LastMovementInput.Normalize();
	if( FMath::IsNearlyEqual(AxisValue,0))
	{
		bIsMovingForward = false;
	}
	else
	{
		bWantsToGrabWeapon = false;
		bIsMovingForward = true;
	}
	
	if(FMath::IsNearlyEqual(AxisValue,0) && !bIsMovingRight && !bWantsToGrabWeapon)
	{
		ChangeMovementState(EPlayerMovementState::Idle);
		bIsMovingForward = false;
	}
	else if ( !(CurrentPlayerMovementState == EPlayerMovementState::Running) && !(CurrentPlayerMovementState == EPlayerMovementState::Walking))
	{
		if(bIsHoldingRunButton)
		{
			ChangeMovementState(EPlayerMovementState::Running);
		}
		else
		{
			ChangeMovementState(EPlayerMovementState::Walking);
		}
		
	}

	if(AxisValue )
	{
		

		FVector Direction;
		if (CurrentPlayerFightState == EPlayerFightState::Aim)
		{
			Direction = CalculateMovementYaw(AxisValue, true, true);
			AddMovementInput(Direction.GetSafeNormal(), AxisValue);
		}
		else
		{
			Direction = CalculateMovementYaw(AxisValue, true,false);
			AddMovementInput(Direction.GetSafeNormal(), FMath::Abs(AxisValue));
		}

		
	}
	
	
}

void AShooterCharacter::MoveRight(float AxisValue)
{
	LastMovementInput.Y= AxisValue;
	//LastMovementInput.Normalize();

	if(!bIsRightMoveEnabled)
	{
		return;
	}
	
	RightAxis = AxisValue;
	if (FMath::IsNearlyEqual(AxisValue, 0))
	{
		bIsMovingRight = false;
	}
	else
	{
		bWantsToGrabWeapon = false;
		bIsMovingRight = true;
	}

	if (FMath::IsNearlyEqual(AxisValue, 0) && !bIsMovingForward && !bWantsToGrabWeapon)
	{
		ChangeMovementState(EPlayerMovementState::Idle);

	}
	else if (!(CurrentPlayerMovementState == EPlayerMovementState::Running) && !(CurrentPlayerMovementState == EPlayerMovementState::Walking))
	{
		ChangeMovementState(EPlayerMovementState::Walking);
	}
	
	if(AxisValue && bIsRightMoveEnabled)
	{
		if(CurrentPlayerCoverState == EPlayerCoverState::None) // Not in the cover
		{
			FVector Direction;
			if(CurrentPlayerFightState ==EPlayerFightState::Aim )
			{
				Direction = CalculateMovementYaw(AxisValue, false, true);
				AddMovementInput(Direction.GetSafeNormal(), AxisValue);
			}
			else
			{
				Direction = CalculateMovementYaw(AxisValue, false,false);
				AddMovementInput(Direction.GetSafeNormal(), FMath::Abs(AxisValue));
			}
			
			
			
		}
		else
		{
			//Since we are changing the rotation Axis Value need to be multiplied by -1

			
			
			AddMovementInput(CoverRightDirection, AxisValue );
		}
	}
	

}

void AShooterCharacter::LookUpRate(float AxisValue)
{
	AddControllerPitchInput(AxisValue * RotationRate *  GetWorld()->GetDeltaSeconds());
}


void AShooterCharacter::LookRightRate(float AxisValue)
{
	
	
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Jump()
{
	Super::Jump();
	ChangeMovementState(EPlayerMovementState::Jumping);
}

void AShooterCharacter::CrouchAndCoverStart()
{
	

	if(CurrentPlayerCoverState != EPlayerCoverState::None) // flip flop!
	{
		ChangeCoverState(EPlayerCoverState::None);
		return;
	}
	else
	{
		const FCoverQuery CoverQuery = CoverComponent->EnvironmentalQuery();
		if (CoverQuery.bHitMAx)
		{
			CoverRightDirection = FVector::CrossProduct(CoverQuery.HitMAx.ImpactNormal, FVector::UpVector);
			CoverRightDirection.Normalize();
			
			ChangeCoverState(EPlayerCoverState::StandingCover);
			SetActorRotation(CoverQuery.HitMAx.ImpactNormal.Rotation());
			
		}
		else if (CoverQuery.bHitMin)
		{
			CoverRightDirection = FVector::CrossProduct(CoverQuery.HitMin.ImpactNormal, FVector::UpVector);
			CoverRightDirection.Normalize();

			ChangeCoverState(EPlayerCoverState::CrouchCover);
		}
		else
		{

			//TODO Crouch
		}
	}
	
	

	
}

void AShooterCharacter::CrouchAndCoverEnd()
{
	
}

void AShooterCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	//TODO
	//ChangeMovementState(EPlayerMovementState::Walking);
}

float AShooterCharacter::GetRightAxis() const
{
	return RightAxis;
}


void AShooterCharacter::ChangeMovementState(EPlayerMovementState NewMovementState)
{

	CurrentPlayerMovementState = NewMovementState;
	
	switch (NewMovementState)
	{


	case EPlayerMovementState::Idle:
		{
			
			break;
			
		}
	case EPlayerMovementState::Walking:
		{
			//Handle Orient Movement in animBP
			
			UpdateWalkSpeed();
			
			break;
		}
		
	case EPlayerMovementState::Running:
		
		{
			
			UpdateWalkSpeed();
			break;
		}

		
	case EPlayerMovementState::Jumping:
		{
			break;
		}
	}

	
	UpdateEndFOV();

}



/* The Controller Settings Set the Idle States
 * This Could Be Change by animation instance
 *
 * 
 */
void AShooterCharacter::ChangeFightState(EPlayerFightState NewFightState)
{
	
	CurrentPlayerFightState = NewFightState;
	switch (NewFightState)
	{
		case EPlayerFightState::NotArmed:
		{
			SpeedDecelerator = NotArmedSpeedCoefficient;
			UpdateWalkSpeed();
				
			if (CurrentPlayerMovementState == EPlayerMovementState::Idle)
			{

				GetCharacterMovement()->bOrientRotationToMovement = false;
				GetCharacterMovement()->bUseControllerDesiredRotation = false;
			}
			else
			{

				GetCharacterMovement()->bOrientRotationToMovement = true;
				GetCharacterMovement()->bUseControllerDesiredRotation = false;
			}
			bUseControllerRotationYaw = false;
			break;
		}
		case EPlayerFightState::Armed:
		{
			SpeedDecelerator = ArmedSpeedCoefficient;
			UpdateWalkSpeed();
				
			if (CurrentPlayerMovementState == EPlayerMovementState::Idle)
			{

				GetCharacterMovement()->bOrientRotationToMovement = false;
				GetCharacterMovement()->bUseControllerDesiredRotation = false;
			}
			else
			{

				GetCharacterMovement()->bOrientRotationToMovement = true;
				GetCharacterMovement()->bUseControllerDesiredRotation = false;
			}
			bUseControllerRotationYaw = false;
			break;
		}
		case EPlayerFightState::Aim:
		{
			//Enter to strafe mode
			SpeedDecelerator = AimSpeedCoefficient;
			UpdateWalkSpeed();
				
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			bUseControllerRotationYaw = true;
			break;
		}

	}
	
	APlayerCameraManager* PlayerCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (PlayerCamera)
	{
		PlayerCamera->PlayCameraShake(CameraShake, 1);
	}
	
	UpdateEndFOV();

}

void AShooterCharacter::ChangeCoverState(EPlayerCoverState NewCoverState)
{
	
	CurrentPlayerCoverState = NewCoverState;


	if(NewCoverState == EPlayerCoverState::None)
	{

		//Set actor rotation based on being right or left
		UBasePlayerAnimInstance* PlayerAnimInstance = Cast<UBasePlayerAnimInstance>(GetMesh()->GetAnimInstance());
		const FVector NewActorForward = PlayerAnimInstance->IsCharacterFacingRight() ? CoverRightDirection : -1 * CoverRightDirection;
		SetActorRotation(NewActorForward.Rotation());


		
		GetCharacterMovement()->MaxAcceleration = NormalAcceleration;
		ChangeFightState(CurrentPlayerFightState);
		bIsForwardMoveEnabled = true;
	}
	else
	{
		ChangeMovementState(EPlayerMovementState::Idle);
		GetCharacterMovement()->MaxAcceleration = CrouchAcceleration;

		
		//TODO better logic?
		
		
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		bUseControllerRotationYaw = false;
		
		bIsForwardMoveEnabled = false;
		
	}

	
	//TODO camera setting and WalkSpeed
	switch (NewCoverState)
	{
		case EPlayerCoverState::None:
			{
				
				break;
			}
		case EPlayerCoverState::CrouchCover:
			{
				
				
				GetCharacterMovement()->Crouch();
				
				break;
			}
		case EPlayerCoverState::StandingCover:
			{

				//TODO this is temporary and needs to be change in changewalkspeed function
				//TODO add new walk speed for standing cover
				GetCharacterMovement()->MaxWalkSpeed = MaxCrouchSpeed;
				
				break;
			}
		case EPlayerCoverState::LeftPeekCover:
			{
				
				break;
			}
		case EPlayerCoverState::RightPeekCover:
			{
				
				break;
			}
	}

	UpdateWalkSpeed();
}




void AShooterCharacter::UpdateWalkSpeed()
{
	if(CurrentPlayerCoverState == EPlayerCoverState::None)
	{
		switch (CurrentPlayerMovementState)
		{
		case EPlayerMovementState::Walking:
		{
			GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed - SpeedDecelerator;
			break;
		}

		case EPlayerMovementState::Running:
		{
			GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed - SpeedDecelerator;
			break;
		}
		default:;
		}
	}
	else
	{
		//TODO Update Speed Based On Cover
	}
	

	
}


void AShooterCharacter::ChangeCameraFOV()
{
	APlayerCameraManager* PlayerCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if(PlayerCamera)
	{
		float targetFOV = FMath::FInterpTo(PlayerCamera->GetFOVAngle(), EndFOV,GetWorld()->GetDeltaSeconds(), FOVSpeed);
		if (FMath::Abs(targetFOV - EndFOV) < 0.1)
		{
			if (GetWorldTimerManager().IsTimerActive(FOVTimerHandle))
			{
				GetWorldTimerManager().ClearTimer(FOVTimerHandle);
			}
			targetFOV = EndFOV;
			
		}

		PlayerCamera->SetFOV(targetFOV);
	}
	else
	{
		if(GetWorldTimerManager().IsTimerActive(FOVTimerHandle))
		{
			GetWorldTimerManager().ClearTimer(FOVTimerHandle);
		}
	}
	
}

void AShooterCharacter::UpdateEndFOV()
{

	bool IsFovChanged = false;
	
	if (CurrentPlayerFightState == EPlayerFightState::Aim)
	{
		if (EndFOV != AimFOV)
		{
			IsFovChanged = true;
			FOVSpeed = AimFOVSpeed;
			EndFOV = AimFOV;
		}

	}
	else if (CurrentPlayerMovementState == EPlayerMovementState::Running)
	{
		if (EndFOV != RunningFOV)
		{
			IsFovChanged = true;
			EndFOV = RunningFOV;
			FOVSpeed = RunFOVSpeed;
		}
		
	}
	else
	{
		if (EndFOV != DefaultFOV)
		{
			IsFovChanged = true;

			FOVSpeed = RunFOVSpeed;
			EndFOV = DefaultFOV;
		}

	}


	if (!GetWorldTimerManager().IsTimerActive(FOVTimerHandle) && IsFovChanged)
	{
		GetWorldTimerManager().SetTimer(FOVTimerHandle, this, &AShooterCharacter::ChangeCameraFOV, GetWorld()->DeltaTimeSeconds, true, 0.f);
	}
}

FVector AShooterCharacter::CalculateMovementYaw(float AxisValue, bool IsMoveingForward,bool IsAiming)
{
	FVector Direction;

	if(IsAiming)
	{
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation = FRotator(0, Rotation.Yaw, 0);
		if(IsMoveingForward)
		{
			Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X).GetSafeNormal();
		}
		else
		{
			Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y).GetSafeNormal();

		}
		
	}
	else
	{
		const FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation = FRotator(0, Rotation.Yaw, 0);
		YawRotation.Yaw = AxisValue > 0 ? YawRotation.Yaw : YawRotation.Yaw - 180;
		FVector TargetDirection;
		if (IsMoveingForward)
		{
			TargetDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X).GetSafeNormal();
		}
		else
		{
			TargetDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y).GetSafeNormal();
		}
		const FRotator PlayerRotation = GetActorRotation();
		const FRotator PlayerYawRotation = FRotator(0, PlayerRotation.Yaw, 0);
		const FVector CurrentDirection = FRotationMatrix(PlayerYawRotation).GetUnitAxis(EAxis::X).GetSafeNormal();
		Direction = FMath::VInterpTo(CurrentDirection, TargetDirection, GetWorld()->GetDeltaSeconds(), MovementLerpSpeed);
		
	}
	

	return Direction;
	
}

EPlayerMovementState AShooterCharacter::GetMovementState()
{
	return CurrentPlayerMovementState;
}

EPlayerFightState AShooterCharacter::GetFightState()
{
	return CurrentPlayerFightState;
}

EPlayerCoverState AShooterCharacter::GetCoverState()
{
	return CurrentPlayerCoverState;
}


void AShooterCharacter::RunStart()
{
	bIsHoldingRunButton = true;
	if(bIsCrouched)
	{
		
	}
	ChangeMovementState(EPlayerMovementState::Running);
}

void AShooterCharacter::RunEnd()
{
	bIsHoldingRunButton = false;
	ChangeMovementState(EPlayerMovementState::Walking);
}

void AShooterCharacter::AimStart()
{

	bIsHoldingAimButton = true;
	
	if (GunInventory.Num() > 0)
	{
		ChangeFightState(EPlayerFightState::Aim);
	}
}

void AShooterCharacter::AimEnd()
{
	bIsHoldingAimButton = false;
	if(GunInventory.Num() > 0)
	{
		ChangeFightState(EPlayerFightState::Armed);
	}
	else
	{
		ChangeFightState(EPlayerFightState::NotArmed);
	}
	
}

void AShooterCharacter::CrouchInput()
{
	if(!bIsCrouched)
	{
		 Super::Crouch();
	}
	else
	{
		Super::UnCrouch();
	}
}

void AShooterCharacter::ShootStart()
{
	Shoot();
}

void AShooterCharacter::EquipWeaponInput()
{

	WeaponToBeGrabbed = CanEquipWeapon();


	if (WeaponToBeGrabbed)
	{
		FVector WeaponLocation = WeaponToBeGrabbed->GetActorLocation();
		WeaponLocation.Z = 0;
		FVector PlayerLocation = GetActorLocation();
		PlayerLocation.Z = 0;
		const FVector ActorToGun = WeaponLocation - PlayerLocation;
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "ActorToGun Lenght: " + FString::SanitizeFloat(ActorToGun.Size()));
		if (ActorToGun.Size() < 130.f)
		{
			bAutoGrabWeapon = false;
			GrabbingTheWeapon();
		}
		else
		{
			
			if(GetMovementComponent()->Velocity.Size() < 50)
			{
				
				GetMovementComponent()->StopMovementImmediately();
			}
			bAutoGrabWeapon = true;
			bWantsToGrabWeapon = true;
		}
	}
	else
	{
		bWantsToGrabWeapon = false;
	}


}


AGun* AShooterCharacter::CanEquipWeapon()
{
	AGun* EquipGunHandle = nullptr;
	if (GunInventory.Num() >= MaxGunThatCanBeCarried)
	{
		UE_LOG(LogTemp, Warning, TEXT("Your Gun Inventory Is Full"));
	}
	else
	{
		TArray<AActor* > OverlappedGuns;
		GetOverlappingActors(OverlappedGuns, GunClass);
		if (OverlappedGuns.Num() > 0)
		{
			EquipGunHandle = FindClosestWeapon(OverlappedGuns);
		}
	}
	return EquipGunHandle;
}


AGun* AShooterCharacter::FindClosestWeapon(TArray<AActor*>& OverlappedGuns)
{

	AGun* EquipedGun = Cast<AGun>(OverlappedGuns[0]);
	float minDistance = FVector::Distance(OverlappedGuns[0]->GetActorLocation(), GetActorLocation());
	for (int i = 1; i < OverlappedGuns.Num(); i++)
	{
		float newDis = FVector::Distance(OverlappedGuns[i]->GetActorLocation(), GetActorLocation());
		if (newDis < minDistance)
		{
			minDistance = newDis;
			EquipedGun = Cast<AGun>(OverlappedGuns[i]);
		}
	}

	return  EquipedGun;
}

void AShooterCharacter::GoToNearestWeaponLocation()
{
	if (WeaponToBeGrabbed)
	{
		FVector WeaponLocation = WeaponToBeGrabbed->GetActorLocation();
		WeaponLocation.Z = 0;
		FVector PlayerLocation = GetActorLocation();
		PlayerLocation.Z = 0;
		const FVector ActorToGun = WeaponLocation - PlayerLocation;
		GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Green, "ActorToGun Lenght: " + FString::SanitizeFloat(ActorToGun.Size()));
		if (ActorToGun.Size() > 80.f)
		{
			if(CurrentPlayerMovementState == EPlayerMovementState::Idle)
			{
				ChangeMovementState(EPlayerMovementState::Walking);
			}
			AddMovementInput(ActorToGun.GetSafeNormal(),1);
		}
		else
		{
			
			GrabbingTheWeapon();
			bWantsToGrabWeapon = false;
		}
	}

}

void AShooterCharacter::EquipWeapon()
{

	if(WeaponToBeGrabbed)
	{
		UE_LOG(LogTemp, Warning, TEXT("New Gun Equiped"));
		WeaponToBeGrabbed->Equip();
		GunInventory.Add(WeaponToBeGrabbed);

		//todo is this still valid?
		if(bIsHoldingAimButton)
		{
			ChangeFightState(EPlayerFightState::Aim);
		}
		else
		{
			ChangeFightState(EPlayerFightState::Armed);
		}
		
		

		if (GunInventory.Num() == 1)
		{
			AttachGun(TEXT("WeaponSocket"), 1);
			CurrentGunIndex = 0;
			//set gun Owner???
		}
		else if (GunInventory.Num() == 2)
		{
			AttachGun(TEXT("SecondWeaponSocket"), 2);
		}
	}
}




bool AShooterCharacter::HasWeaponInHand()
{
	return GunInventory.Num() > 0;
}

bool AShooterCharacter::IsReloading()
{
	if(GunInventory.Num() > 0)
	{
		return GunInventory[CurrentGunIndex]->IsReloading();
	}
	return false;
}

AGun* AShooterCharacter::GetCurrentWeaponRefrence()
{
	if(GunInventory.Num() > 0)
	{
		return GunInventory[CurrentGunIndex];
	}
	else
	{
		return nullptr;
	}
}


void AShooterCharacter::DropWeapon()
{
	if(GunInventory.Num() > 0 )
	{
		if(!GunInventory[CurrentGunIndex]->IsReloading())
		{
			const int prevGunIndex = CurrentGunIndex;
			GunInventory[prevGunIndex]->Drop();
			ChangeGunInInventory();
			GunInventory[prevGunIndex]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			GunInventory.RemoveAt(prevGunIndex);
			CurrentGunIndex = GunInventory.Num() - 1;

			if (CurrentGunIndex == -1)
			{
				ChangeFightState(EPlayerFightState::NotArmed);
			}
		}
		
	}
	
}


void AShooterCharacter::Reload()
{

	if(GunInventory.Num() != 0)
	{
		if(GunInventory[CurrentGunIndex])
		{
			GunInventory[CurrentGunIndex]->Reload();
			//GunInventory[CurrentGunIndex]->SetbIsReloading(true);
			
		}
	
	}
	
	
}

void AShooterCharacter::ChangeGunInInventory()
{

	if(GunInventory.Num() == 2)
	{
		AttachGun("SecondWeaponSocket",CurrentGunIndex + 1);
		CurrentGunIndex = (CurrentGunIndex + 1) % 2;
		AttachGun("WeaponSocket",CurrentGunIndex + 1);
	}

	
}

void AShooterCharacter::AttachGun(FName SocketName, int32 GunNumber)
{

	GunInventory[GunNumber - 1]->SetOwner(GetMesh()->GetOwner());
	GunInventory[GunNumber - 1]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
} 


void AShooterCharacter::Shoot()
{

	if (GetFightState() != EPlayerFightState::Aim)
	{
		ShootEnd();

		return;
	}
	if (GunInventory.Num() != 0)
	{
		if (GunInventory[CurrentGunIndex])
		{
			GunInventory[CurrentGunIndex]->PullTrigger();
		}
	}

}

float AShooterCharacter::GetDefaultMaxWalkSpeed()
{
	return MaxWalkSpeed;
}

FVector AShooterCharacter::GetLastMovementInput()
{
	return LastMovementInput;
}

bool AShooterCharacter::GetIsRightMoveEnable()
{
	return bIsRightMoveEnabled;
}

bool AShooterCharacter::GetIsForwardMoveEnable()
{
	return bIsForwardMoveEnabled;
}

void AShooterCharacter::SetMovementEnable(bool bEnable)
{
	if(CurrentPlayerCoverState == EPlayerCoverState::None)
	{
		bIsForwardMoveEnabled = bEnable;
	}
	else
	{
		bIsForwardMoveEnabled = false;
	}

	
	bIsRightMoveEnabled = bEnable;
	
}



void AShooterCharacter::ShootEnd()
{
	if (GunInventory.Num() != 0)
	{
		if (GunInventory[CurrentGunIndex])
		{
			GunInventory[CurrentGunIndex]->ReleaseTrigger();
		}
	}

}






void AShooterCharacter::FlipFlopCharacterDebug()
{
	bCharacterDebug = !bCharacterDebug;
}

void AShooterCharacter::FlipFlopGunDebug()
{
	bGunDebug = !bGunDebug;
}

void AShooterCharacter::FlipFlopAnimationDebug()
{
	bAnimationDebug = !bAnimationDebug;
}

void AShooterCharacter::CharacterDebug()
{
	int32 PlayerSpeed = FVector::DotProduct(GetVelocity(), GetActorRotation().Vector());

	//CurrentPlayerMovementState
	const FString MovementEnumString = UEnum::GetValueAsString<EPlayerMovementState>(GetMovementState());
	const FString FightEnumString = UEnum::GetValueAsString<EPlayerFightState>(GetFightState());
	const FString CoverEnumString = UEnum::GetValueAsString<EPlayerCoverState>(CurrentPlayerCoverState);



	const FVector ActorForwardVectorNormalize = GetActorForwardVector().GetSafeNormal();


	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "Speed: " + GetVelocity().ToString());
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "Location: " + GetActorLocation().ToString());


	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "ActorForwardVector: " + ActorForwardVectorNormalize.ToString());

	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "LastMovementInput: " + LastMovementInput.ToString());
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "Mesh Component Velocity: " + GetMesh()->GetComponentVelocity().ToString());


	FVector StartLoc1 = GetActorLocation();
	FVector EndLoc1 = StartLoc1 + GetActorForwardVector() * 150;
	DrawDebugLine(GetWorld(), StartLoc1, EndLoc1, FColor::Magenta);
	


	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "MovementMode: " + MovementEnumString);
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "FightMode: " + FightEnumString);
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "CoverMode: " + CoverEnumString);
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "GunIndex:  " + FString::FromInt(CurrentGunIndex));
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "OrientRotationToMovement:  " + FString((GetCharacterMovement()->bOrientRotationToMovement ? "true" : "false")));
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "UseControllerDesiredRotation:  " + FString(GetCharacterMovement()->bUseControllerDesiredRotation ? "true" : "false"));
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "UseControllerRotationYaw:  " + FString((bUseControllerRotationYaw ? "true" : "false")));

	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "Is timer active:  " + FString((GetWorldTimerManager().IsTimerActive(FOVTimerHandle) ? "true" : "false")));
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "End FOV:  " + FString::SanitizeFloat(EndFOV));
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "Move Forward Enable:  " + FString(bIsForwardMoveEnabled ? "True" : "False"));
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "Move Right Enable:  " + FString(bIsRightMoveEnabled ? "True" : "False"));
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "Actor Rotation: " + GetActorRotation().ToString());
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "Mesh Rotation: " + GetMesh()->GetRelativeRotation().ToString());
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "Controller Rotation: " + GetControlRotation().ToString());

	APlayerCameraManager* PlayerCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (PlayerCamera)
	{
		

		GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, "Camera FOV:  " + FString::SanitizeFloat(PlayerCamera->GetFOVAngle()));

	}

}
