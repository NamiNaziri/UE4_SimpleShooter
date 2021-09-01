// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SimpleShooter/Enums/EPlayerMovementState.h"
#include "SimpleShooter/Enums/EPlayerFightState.h"
#include "SimpleShooter/Enums/EPlayerCoverState.h"
#include "SimpleShooter/Player/ActorComponents/CoverComponent.h"
#include "ShooterCharacter.generated.h"





class AGun;
class UCharacterMovementComponent;

UCLASS()
class SIMPLESHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure)
		bool IsDead() const;

	UFUNCTION(BlueprintPure)
		bool IsAccelerating() const;

	
	UFUNCTION(BlueprintPure)
		float GetHealthPercentage() const;
	
	UFUNCTION(BlueprintPure)
		bool GetAnimationDebug() const;

	UFUNCTION(BlueprintPure)
		EPlayerMovementState GetMovementState();
	UFUNCTION(BlueprintPure)
		EPlayerFightState GetFightState();
	UFUNCTION(BlueprintPure)
		EPlayerCoverState GetCoverState();
	UFUNCTION(BlueprintPure)
		float GetDefaultMaxWalkSpeed();
	UFUNCTION(BlueprintPure)
		FVector GetLastMovementInput();
	UFUNCTION(BlueprintPure)
		bool GetIsRightMoveEnable();
	UFUNCTION(BlueprintPure)
		bool GetIsForwardMoveEnable();
	
	UFUNCTION(BlueprintCallable)
		void SetMovementEnable(bool bEnable);


	UFUNCTION()
	virtual void Landed(const FHitResult& Hit) override;

	float GetRightAxis() const;
	//******************WEAPON******************//
	

	

	UFUNCTION(BlueprintCallable)
		void EquipWeaponInput();
	
	UFUNCTION(BlueprintCallable)
		void EquipWeapon();

	UFUNCTION(BlueprintCallable)
		void GoToNearestWeaponLocation();
	
	UFUNCTION(BlueprintCallable)
		AGun* CanEquipWeapon();

	UFUNCTION(BlueprintImplementableEvent)
		void GrabbingTheWeapon();

	UFUNCTION(BlueprintCallable)
		bool HasWeaponInHand();

	UFUNCTION(BlueprintCallable)
		bool IsReloading();

	UFUNCTION(BlueprintCallable)
		AGun* GetCurrentWeaponRefrence();
	
	UFUNCTION(BlueprintCallable, Category = Character)
		void Shoot();


	//Cover
	//UPROPERTY(Category = Cover, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPROPERTY()
		UCoverComponent* CoverComponent;
	UPROPERTY()
		FVector CoverRightDirection;
	
private:

	//******************INPUT FUNCTIONS******************//
	UFUNCTION(BlueprintCallable, Category = Character)
		void Moveforward(float AxisValue);
	
	UFUNCTION(BlueprintCallable, Category = Character)
		void MoveRight(float AxisValue);
	
	UFUNCTION(BlueprintCallable, Category = Character)
		void LookUpRate(float AxisValue);
	
	UFUNCTION(BlueprintCallable, Category = Character)
		void LookRightRate(float AxisValue);
	
	UFUNCTION(BlueprintCallable, Category = Character)
		void RunStart();
	
	UFUNCTION(BlueprintCallable, Category = Character)
		void RunEnd();

	UFUNCTION(BlueprintCallable, Category = Character)
		void CrouchInput();
	
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void AimStart();
	
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void AimEnd();
	
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void ShootStart();
	
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void ShootEnd();

	UFUNCTION(BlueprintCallable, Category = Weapon)
		void Reload();

	virtual void Jump() override;

	UFUNCTION(BlueprintCallable, Category = Cover)
		void CrouchAndCoverStart();

	UFUNCTION(BlueprintCallable, Category = Cover)
		void CrouchAndCoverEnd();
	
	
	//******************Character Status******************//
	void ChangeMovementState(EPlayerMovementState NewMovementState);
	void ChangeFightState(EPlayerFightState NewFightState);
	void ChangeCoverState(EPlayerCoverState NewCoverState);

	//Changes walk speed based on current movement state
	void UpdateWalkSpeed();

	UFUNCTION(BlueprintCallable, Category = Weapon)
		void DropWeapon();
	
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void ChangeGunInInventory();
	
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void AttachGun(FName SocketName, int32 GunNumber);

	UFUNCTION(BlueprintCallable, Category = Weapon)
		AGun*  FindClosestWeapon(TArray<AActor* >& OverlappedGuns);

	//interpolates smoothly form current camera fov to EndFOV and is used by timers
	UFUNCTION(BlueprintCallable, Category = Camera)
		void ChangeCameraFOV();

	//Updates EndFov based on current movement and fight state and enables the timer 
	UFUNCTION(BlueprintCallable, Category = Camera)
		void UpdateEndFOV();

	UFUNCTION(BlueprintCallable, Category = Movement)
		FVector CalculateMovementYaw(float AxisValue, bool IsMoveForward, bool IsAiming);

	UPROPERTY(EditAnywhere, Category = "Expriment", meta = (AllowPrivateAccess = "true"))
		float MovementLerpSpeed = 5;
	
	float RightAxis; //right axis input

	
	
	UPROPERTY(EditAnywhere)
		float RotationRate = 70;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> GunClass;

	TArray<AGun*> GunInventory;

	AGun* WeaponToBeGrabbed;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly , Category = "GunFight" ,  meta=(AllowPrivateAccess = "true"))
	int32 MaxGunThatCanBeCarried;
	//-1 means no Weapon
	int32 CurrentGunIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunFight", meta = (AllowPrivateAccess = "true"))
		bool bWantsToGrabWeapon;

	UPROPERTY(BlueprintReadOnly, Category = "GunFight", meta = (AllowPrivateAccess = "true"))
		bool bAutoGrabWeapon;
	

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;
	UPROPERTY(VisibleAnywhere)
	float Health = 0;
	

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly , Category = "Movement" ,  meta=(AllowPrivateAccess = "true"))
	float MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly , Category = "Movement" ,  meta=(AllowPrivateAccess = "true"))
	float MaxRunSpeed;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly , Category = "Movement" ,  meta=(AllowPrivateAccess = "true"))
	float MaxCrouchSpeed;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly , Category = "Movement" ,  meta=(AllowPrivateAccess = "true"))
	float NotArmedSpeedCoefficient;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly , Category = "Movement" ,  meta=(AllowPrivateAccess = "true"))
	float ArmedSpeedCoefficient;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly , Category = "Movement" ,  meta=(AllowPrivateAccess = "true"))
	float AimSpeedCoefficient;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float NormalAcceleration;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float CrouchAcceleration;
	


	//Expriment
	FVector LastMovementInput;
	
	UPROPERTY()
	float SpeedDecelerator = 0;

	EPlayerMovementState CurrentPlayerMovementState;
	EPlayerFightState CurrentPlayerFightState;
	EPlayerCoverState CurrentPlayerCoverState;
	float CurrentSpeedCoefficient;


	bool bIsMovingForward;
	bool bIsMovingRight;
	bool bIsHoldingAimButton;
	bool bIsHoldingRunButton;
	bool bIsRightMoveEnabled;
	bool bIsForwardMoveEnabled;

	
	UPROPERTY(BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true"))
		bool bCharacterDebug;

	UPROPERTY(BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true"))
		bool bGunDebug;

	UPROPERTY(BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true"))
		bool bAnimationDebug;


	
	//Camera Settings//
	UPROPERTY()
	FTimerHandle FOVTimerHandle;
	UPROPERTY()
		float EndFOV;
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		float DefaultFOV = 90;
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		float AimFOV = 60;
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		float RunningFOV = 75;
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		float AimFOVSpeed = 10;
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		float RunFOVSpeed = 1;
	UPROPERTY()
		float FOVSpeed = 1;
	


	
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMatineeCameraShake> CameraShake;



	void FlipFlopCharacterDebug();
	void FlipFlopGunDebug();
	void FlipFlopAnimationDebug();
	void CharacterDebug();
};


