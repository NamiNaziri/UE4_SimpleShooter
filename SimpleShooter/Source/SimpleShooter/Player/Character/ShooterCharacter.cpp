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

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Initialization

	CurrentPlayerMovementState = EPlayerMovementState::Idle;
	CurrentPlayerFightState = EPlayerFightState::NotArmed;

	MaxWalkSpeed = 300;
	MaxRunSpeed = 600;
	MaxCrouchSpeed = 150;
	FOVSpeed = 1;
	NotArmedSpeedCoefficient = 0;
	ArmedSpeedCoefficient = 50;
	AimSpeedCoefficient = 150;
	SpeedDecelerator = NotArmedSpeedCoefficient;
	CurrentGunIndex = -1;
	bIsHoldingAimButton = false;

	CurrentSpeedCoefficient = NotArmedSpeedCoefficient;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed - SpeedDecelerator;
	GetCharacterMovement()->MaxWalkSpeedCrouched = MaxCrouchSpeed - SpeedDecelerator;
	
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

	

	

}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if(bWantsToGrabWeapon)
	{
		GoToNearestWeaponLocation();
	}
	
	if(bCharacterDebug)
	{
		int32 PlayerSpeed = FVector::DotProduct(GetVelocity(), GetActorRotation().Vector());

		//CurrentPlayerMovementState
		const FString MovementEnumString = UEnum::GetValueAsString<EPlayerMovementState>(GetMovementState());
		const FString FightEnumString = UEnum::GetValueAsString<EPlayerFightState>(GetFightState());



		const FVector ActorForwardVectorNormalize = GetActorForwardVector().GetSafeNormal();
		const FVector LastMovementInputVectorNormalize = GetLastMovementInputVector().GetSafeNormal();


		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Magenta, "Speed: " + GetVelocity().ToString());
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Magenta, "Location: " + GetActorLocation().ToString());


		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Magenta, "ActorForwardVector: " + ActorForwardVectorNormalize.ToString());

		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Magenta, "LastMovementInput: " + LastMovementInputVectorNormalize.ToString());
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Magenta, "Mesh Component Velocity: " + GetMesh()->GetComponentVelocity().ToString());


		FVector StartLoc1 = GetActorLocation();
		FVector EndLoc1 = StartLoc1 + GetActorForwardVector() * 150;
		DrawDebugLine(GetWorld(), StartLoc1, EndLoc1, FColor::Magenta);



		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Magenta, "MovementMode: " + MovementEnumString);
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Magenta, "FightMode: " + FightEnumString);
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Magenta, "GunIndex:  " + FString::FromInt(CurrentGunIndex));
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Magenta, "OrientRotationToMovement:  " + FString((GetCharacterMovement()->bOrientRotationToMovement ? "true" : "false")));
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Magenta, "UseControllerDesiredRotation:  " + FString(GetCharacterMovement()->bUseControllerDesiredRotation ? "true" : "false"));
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Magenta, "UseControllerRotationYaw:  " + FString((bUseControllerRotationYaw ? "true" : "false")));

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
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AShooterCharacter::LookRightRate);


	PlayerInputComponent->BindAction(TEXT("Jump"),  EInputEvent::IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Shoot);
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

float AShooterCharacter::GetHealtPercent() const
{
	return Health / MaxHealth;
}

float AShooterCharacter::GetRightAxis() const
{
	return RightAxis;
}

bool AShooterCharacter::GetAnimationDebug() const
{
	return bAnimationDebug;
}

void AShooterCharacter::Moveforward(float AxisValue)
{
	
	

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
		ChangeMovementState(EPlayerMovementState::Walking);
	}

	
	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X).GetSafeNormal();
	//AddMovementInput(Direction, AxisValue);
	
	
	AddMovementInput(Direction.GetSafeNormal(), AxisValue);
	
}

void AShooterCharacter::MoveRight(float AxisValue)
{

	
	
	RightAxis = AxisValue;
	if(FMath::IsNearlyEqual(AxisValue,0))
	{
		bIsMovingRight = false;
	}
	else
	{
		bWantsToGrabWeapon = false;
		bIsMovingRight = true;
	}
	
	if(FMath::IsNearlyEqual(AxisValue,0) && !bIsMovingForward && !bWantsToGrabWeapon)
	{
		ChangeMovementState(EPlayerMovementState::Idle);
		
	}
	else if ( !(CurrentPlayerMovementState == EPlayerMovementState::Running) && !(CurrentPlayerMovementState == EPlayerMovementState::Walking))
	{
		ChangeMovementState(EPlayerMovementState::Walking);
	}
	// find out which way is right
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	// get right vector 
	const FVector Direction = FRotationMatrix(YawRotation.GetNormalized()).GetUnitAxis(EAxis::Y).GetSafeNormal();
	// add movement in that direction

    AddMovementInput(Direction.GetSafeNormal(), AxisValue );

}

void AShooterCharacter::LookUpRate(float AxisValue)
{
	AddControllerPitchInput(AxisValue *  GetWorld()->GetDeltaSeconds());
}


void AShooterCharacter::LookRightRate(float AxisValue)
{
	AddControllerYawInput(AxisValue * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Jump()
{
	Super::Jump();
	ChangeMovementState(EPlayerMovementState::Jumping);
}

void AShooterCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	ChangeMovementState(EPlayerMovementState::Walking);
}



void AShooterCharacter::ChangeMovementState(EPlayerMovementState NewMovementState)
{

	CurrentPlayerMovementState = NewMovementState;
	
	switch (NewMovementState)
	{


	case EPlayerMovementState::Idle:
		{
			
			CurrentPlayerMovementState = EPlayerMovementState::Idle;
			break;
			
		}
	case EPlayerMovementState::Walking:
		{
			//Handle Orient Movement in animBP
			
			CurrentPlayerMovementState = EPlayerMovementState::Walking;
			UpdateWalkSpeed(CurrentPlayerMovementState);
			
			break;
		}
		
	case EPlayerMovementState::Running:
		
		{
			CurrentPlayerMovementState = EPlayerMovementState::Running;
			UpdateWalkSpeed(CurrentPlayerMovementState);
			break;
		}

		
	case EPlayerMovementState::Jumping:
		{
			CurrentPlayerMovementState = EPlayerMovementState::Jumping;
			break;
		}
	}


	if(NewMovementState == EPlayerMovementState::Running && CurrentPlayerFightState != EPlayerFightState::Aim)
	{

		EndFOV = RunningFOV;
		FOVSpeed = RunFOVSpeed;
		GetWorldTimerManager().SetTimer(FOVTimerHandle, this, &AShooterCharacter::ChangeFOV, GetWorld()->DeltaTimeSeconds, true, 0.f);
	}
	else if (CurrentPlayerFightState != EPlayerFightState::Aim)
	{
		EndFOV = DefaultFOV;
		FOVSpeed = RunFOVSpeed;
		GetWorldTimerManager().SetTimer(FOVTimerHandle, this, &AShooterCharacter::ChangeFOV, GetWorld()->DeltaTimeSeconds, true, 0.f);

	}

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
			UpdateWalkSpeed(CurrentPlayerMovementState);
				
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
			UpdateWalkSpeed(CurrentPlayerMovementState);
				
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
			UpdateWalkSpeed(CurrentPlayerMovementState);
				
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			bUseControllerRotationYaw = true;
			break;
		}

	}
	
	APlayerCameraManager* PlayerCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	
	
	if(NewFightState == EPlayerFightState::Aim)
	{
		UE_LOG(LogTemp, Warning, TEXT("In aim"));
		if (PlayerCamera)
		{
			PlayerCamera->StartMatineeCameraShake(CameraShake, 1);
			FOVSpeed = AimFOVSpeed;
			EndFOV = AimFOV;
			GetWorldTimerManager().SetTimer(FOVTimerHandle, this, &AShooterCharacter::ChangeFOV, GetWorld()->DeltaTimeSeconds,true,0.f);
			
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("In ELSE"));
		if (PlayerCamera)
		{
			if(CurrentPlayerMovementState == EPlayerMovementState::Running)
			{
				UE_LOG(LogTemp, Warning, TEXT("Run FOV"));
				FOVSpeed = AimFOVSpeed;
				EndFOV = RunningFOV;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Default FOV"));
				FOVSpeed = AimFOVSpeed;
				EndFOV = DefaultFOV;
			}
			
			GetWorldTimerManager().SetTimer(FOVTimerHandle, this, &AShooterCharacter::ChangeFOV, GetWorld()->DeltaTimeSeconds, true, 0.f);
		}
	}
}

void AShooterCharacter::UpdateWalkSpeed(EPlayerMovementState CurrentMovementState)
{
	switch (CurrentMovementState)
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
		default: ;
	}
}


void AShooterCharacter::ChangeFOV()
{
	APlayerCameraManager* PlayerCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if(PlayerCamera)
	{
		const float targetFOV = FMath::FInterpTo(PlayerCamera->GetFOVAngle(), EndFOV,GetWorld()->GetDeltaSeconds(), FOVSpeed);
		if (FMath::IsNearlyEqual(targetFOV, EndFOV))
		{
			if (GetWorldTimerManager().IsTimerActive(FOVTimerHandle))
			{
				GetWorldTimerManager().ClearTimer(FOVTimerHandle);
			}
			
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

EPlayerMovementState AShooterCharacter::GetMovementState()
{
	return CurrentPlayerMovementState;
}

EPlayerFightState AShooterCharacter::GetFightState()
{
	return CurrentPlayerFightState;
}


void AShooterCharacter::RunStart()
{
	if(bIsCrouched)
	{
		
	}
	ChangeMovementState(EPlayerMovementState::Running);
}

void AShooterCharacter::RunEnd()
{
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
	return GunInventory[CurrentGunIndex];
}


void AShooterCharacter::DropWeapon()
{
	if(GunInventory.Num() > 0)
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

void AShooterCharacter::Shoot()
{

	if(GetFightState() != EPlayerFightState::Aim)
	{
		ShootEnd();

		return;
	}
	if(GunInventory.Num() != 0 )
	{
		if(GunInventory[CurrentGunIndex])
		{
			GunInventory[CurrentGunIndex]->PullTrigger();			
		}
	}

}

float AShooterCharacter::GetDefaultMaxWalkSpeed()
{
	return MaxWalkSpeed;
}


void AShooterCharacter::ShootEnd()
{
	if(GunInventory.Num() != 0 )
	{
		if(GunInventory[CurrentGunIndex])
		{
			GunInventory[CurrentGunIndex]->ReleaseTrigger();
		}
	}
	
}


