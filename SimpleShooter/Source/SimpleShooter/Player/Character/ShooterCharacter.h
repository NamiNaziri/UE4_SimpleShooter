// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SimpleShooter/Enums/EPlayerMovementState.h"
#include "SimpleShooter/Enums/EPlayerFightState.h"

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
	float GetHealtPercent() const;

	UFUNCTION(BlueprintPure)
		float GetRightAxis() const;


	UFUNCTION(BlueprintPure)
		bool GetAnimationDebug() const;

	UFUNCTION(BlueprintPure)
	EPlayerMovementState GetMovementState();
	UFUNCTION(BlueprintPure)
	EPlayerFightState GetFightState();

	virtual void Landed(const FHitResult& Hit) override;

	void Shoot();
	float GetDefaultMaxWalkSpeed();

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
	
	
	
private:

	void Moveforward(float AxisValue);
	void MoveRight(float AxisValue);

	void LookUpRate(float AxisValue);
	void LookRightRate(float AxisValue);

	virtual void Jump() override;
	
	void RunStart();
	void RunEnd();

	void AimStart();
	void AimEnd();

	void CrouchInput();


	void ShootEnd();
	void ChangeMovementState(EPlayerMovementState NewMovementState);
	void ChangeFightState(EPlayerFightState NewFightState);
	void UpdateWalkSpeed(EPlayerMovementState CurrentMovementState);

	void DropWeapon();
	void Reload();
	void ChangeGunInInventory();
	void AttachGun(FName SocketName, int32 GunNumber);

	AGun*  FindClosestWeapon(TArray<AActor* >& OverlappedGuns);

	void ChangeFOV();

	void FlipFlopCharacterDebug();
	void FlipFlopGunDebug();
	void FlipFlopAnimationDebug();


	
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


	float RightAxis;

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

	UPROPERTY()
	float SpeedDecelerator = 0;

	EPlayerMovementState CurrentPlayerMovementState;
	EPlayerFightState CurrentPlayerFightState;
	float CurrentSpeedCoefficient;


	bool bIsMovingForward;
	bool bIsMovingRight;
	bool bIsHoldingAimButton;

	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bCharacterDebug;

	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bGunDebug;

	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bAnimationDebug;

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

	
};


