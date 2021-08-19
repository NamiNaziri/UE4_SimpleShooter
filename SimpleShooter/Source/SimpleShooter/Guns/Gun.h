// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UCLASS()
class SIMPLESHOOTER_API AGun : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* PhysicsBox;

	UPROPERTY(VisibleAnywhere)
		UBoxComponent* CollisionBox;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta=(AllowPrivateAccess = "true"));
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlashParticle;
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactEffectParticle;

	UPROPERTY(EditAnywhere)
	USoundBase* MuzzleSound;

	UPROPERTY(EditAnywhere)
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere)
	USphereComponent* EquipmentSphere;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere)
	float Range = 100000;

	UPROPERTY(EditAnywhere)
	float DamageAmount = 10;
	
	UPROPERTY(EditAnywhere)
	int32 MaxAmmoCapacity = 300;
	
	UPROPERTY(EditAnywhere)
	int32 MaxAmmoPerMagazine = 30;

	UPROPERTY(EditAnywhere)
	int32 CurrentAmmoInMagazine = 0; 

	UPROPERTY(EditAnywhere)
	int32 RemainedAmmoInGun = 100;

	UPROPERTY(EditAnywhere)
	float ReloadTime = 2.f;

	UPROPERTY(EditAnywhere)
	float FireRate = 0.5f;

	bool bCanFire = true;
	bool bIsReloading = false;

	//* if Gun has ammo and mag is empty reload automaticly *//
	UPROPERTY(EditAnywhere)
	bool bAutoReload = false;
	
	FTimerHandle ReloadTimerHandle;
	FTimerHandle FireRateTimerHandle;
	FTimerHandle ReleaseTriggerDelayTimerHandle;
	FTimerHandle ReloadDelayTimerHandle;
	FTimerHandle CollisionEnableTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UMatineeCameraShake> CameraFireShake;

private:
	
	bool GunTrace(FHitResult& Hit, FVector& ShotDirection,const AController* PlayerController);
	AController* GetOwnerController() const;

	//UFUNCTION()
    //void EquipmentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void SetSimulateEnable(bool bEnable);
	void SetGunEquipOverlapEventEnable(bool bEnable);
	
public:	
	// Sets default values for this actor's properties
	AGun();
	void PullTrigger();

	void ReleaseTrigger();

	void Reload();

	void Equip();

	void Drop();

	virtual float TakeDamage(float DamageAmount1, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	bool IsReloading();

	USkeletalMeshComponent* GetMesh();
	
	

	//Debug Stuff
	void Printstatistics(float deltaTime);
	float GetReloadTime();
	void SetbIsReloading(bool isReloading);
	
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
