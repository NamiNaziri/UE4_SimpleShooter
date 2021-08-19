// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


#include "Engine/EngineTypes.h"
#include "SimpleShooter/Player/Character/ShooterCharacter.h"
// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	PhysicsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Physics Box Component"));
	SetRootComponent(PhysicsBox);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Component"));
	CollisionBox->SetupAttachment(PhysicsBox);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("scene Component"));
	Root->SetupAttachment(PhysicsBox);
	
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Mesh Component"));
	Mesh->SetupAttachment(PhysicsBox);

	EquipmentSphere = CreateDefaultSubobject<USphereComponent>(TEXT("EquipmentSphere"));
	EquipmentSphere->SetupAttachment(PhysicsBox);

	

 
}


// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	SetSimulateEnable(true);
	
}


void AGun::PullTrigger()
{
	AController* PlayerController = GetOwnerController();
	if (!PlayerController)
	{
		return;
	}
	
	
	if(bCanFire && !bIsReloading)
	{
		bCanFire = false;
		if(CurrentAmmoInMagazine != 0)
		{

			CurrentAmmoInMagazine -= 1;



			//Particle effect
			if (!MuzzleFlashParticle)
			{
				UE_LOG(LogTemp, Warning, TEXT("UGameplayStatics::SpawnEmitterAttached: NULL EmitterTemplate specified!"));
				return;
			}

			UGameplayStatics::SpawnEmitterAttached(MuzzleFlashParticle, Mesh, TEXT("MuzzleFlashSocket"));
			//Sound
			UGameplayStatics::SpawnSoundAttached(MuzzleSound, Mesh, TEXT("MuzzleFlashSocket"));

			//Camera
			APlayerCameraManager* PlayerCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
			PlayerCamera->PlayCameraShake(CameraFireShake, 1);

			
			FVector ShotDirection;
			FHitResult HitResult;
			bool bSuccess = GunTrace(HitResult, ShotDirection, PlayerController);
			if (bSuccess)
			{
				//DrawDebugPoint(GetWorld(), HitResult.Location,20 ,FColor::Red,true);
		
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffectParticle, HitResult.Location - HitResult.Normal * 10, ShotDirection.Rotation());
				UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ImpactSound, HitResult.Location, ShotDirection.Rotation());


				AActor* HittedActor = HitResult.GetActor();
				if (HittedActor)
				{
					FPointDamageEvent DamageEvent(DamageAmount, HitResult, ShotDirection, NULL);
					HittedActor->TakeDamage(DamageAmount, DamageEvent , PlayerController, this);
				}
				
			}
		}
		// there is no else because the ammo could get zero because of the upper part which we have CurrentAmmoInMagazine -=0 
		if(CurrentAmmoInMagazine == 0)
		{

			if(bAutoReload)
			{

				Reload();
				
			}
			if(!bAutoReload)
			{
				if(RemainedAmmoInGun > 0)
				{
					UE_LOG(LogTemp , Warning , TEXT("Please Reload The gun"));
				}
				else
				{
					UE_LOG(LogTemp , Warning , TEXT("No Ammo"));
				}
			}
		}


		GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, [this]() {
                                                            bCanFire = true;
															AShooterCharacter* PlayerCharacter = Cast<AShooterCharacter>(GetOwner());

															if (!PlayerCharacter)
															{
																return;
															}
															PlayerCharacter->Shoot();
                                                        }, FireRate, true);
	}
	else
	{
		if(bIsReloading)
		{
			if(!GetWorld()->GetTimerManager().IsTimerActive(FireRateTimerHandle) && bIsReloading)
			{

				
				//new timer
				GetWorld()->GetTimerManager().SetTimer(ReloadDelayTimerHandle, [this]() {
														AShooterCharacter* PlayerCharacter = Cast<AShooterCharacter>(GetOwner());

														if (!PlayerCharacter)
														{
															return;
														}
														PlayerCharacter->Shoot();
                                                    }, ReloadTime, false);
			}
		}

		
	}
}

void AGun::ReleaseTrigger()
{
	GetWorld()->GetTimerManager().ClearTimer(FireRateTimerHandle);
	bool bIsDelayTimerActive = GetWorld()->GetTimerManager().IsTimerActive(ReleaseTriggerDelayTimerHandle);
	if(!bIsDelayTimerActive)
	{
		GetWorld()->GetTimerManager().SetTimer(ReleaseTriggerDelayTimerHandle, [this]() {
                                                            bCanFire = true;
                                                        }, FireRate, false);
	}
	
}

void AGun::SetGunEquipOverlapEventEnable(bool bEnable)
{
	EquipmentSphere->SetGenerateOverlapEvents(bEnable);
}

void AGun::Reload()
{
	
	if(!bIsReloading)
	{

		
		if(CurrentAmmoInMagazine != MaxAmmoPerMagazine)
		{
			RemainedAmmoInGun += CurrentAmmoInMagazine; 
	
			if(RemainedAmmoInGun >= MaxAmmoPerMagazine)
			{
				CurrentAmmoInMagazine = MaxAmmoPerMagazine;
				RemainedAmmoInGun -= CurrentAmmoInMagazine;
				bIsReloading = true;
			}
			else
			{
				if(RemainedAmmoInGun > 0)
				{
					if( CurrentAmmoInMagazine == RemainedAmmoInGun )
					{
						UE_LOG(LogTemp,Warning, TEXT("No Backup Ammo In Gun"));
					}
					else
					{
						bIsReloading = true;
					}
			
					CurrentAmmoInMagazine = RemainedAmmoInGun;
					RemainedAmmoInGun = 0;
			
				}
				else
				{
					UE_LOG(LogTemp , Warning,TEXT("Gun has no Ammo"));
				}
			}	
		}
		else
		{
			UE_LOG(LogTemp , Warning,TEXT("Your Mag is Full"));
		}

		//TODO Make sure that you are in aim fight and cant change to standing!!
		
		AShooterCharacter* PlayerCharacter = Cast<AShooterCharacter>(GetOwner());

		if (!PlayerCharacter)
		{
			return;
		}


		if(bIsReloading)
		{
			ReloadTime = PlayerCharacter->PlayAnimMontage(ReloadMontage);
			GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, [this]() {
				bIsReloading = false;
				}, ReloadTime, false);
		}
		
	}
}

void AGun::Equip()
{
	SetGunEquipOverlapEventEnable(false);
	SetActorEnableCollision(false);
	SetSimulateEnable(false);



}

void AGun::Drop()
{
	SetGunEquipOverlapEventEnable(true);
	SetSimulateEnable(true);


	GetWorld()->GetTimerManager().SetTimer(CollisionEnableTimerHandle, [this]() {
													SetActorEnableCollision(true);
                                                }, 0.2,false);
	
	
}

float AGun::TakeDamage(float DamageAmount1, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	
	
	PhysicsBox->AddImpulse((GetActorLocation() - DamageCauser->GetActorLocation()).GetSafeNormal() * 8000);
	return 0;

}

bool AGun::IsReloading()
{
	return bIsReloading;
}

USkeletalMeshComponent* AGun::GetMesh()
{
	return Mesh;
}


void AGun::Printstatistics(float deltaTime)
{

	FString Massage = "Ammo: " + FString::FromInt(CurrentAmmoInMagazine) + " / " + FString::FromInt(RemainedAmmoInGun); 
	GEngine->AddOnScreenDebugMessage(-1, deltaTime,FColor::Red, Massage);
	if(bIsReloading)
	{
		GEngine->AddOnScreenDebugMessage(-1, deltaTime,FColor::Red, "Reloading ...");
	}
	
}

float AGun::GetReloadTime()
{
	return ReloadTime;
}

void AGun::SetbIsReloading(bool isReloading)
{
	bIsReloading = isReloading;
}


bool AGun::GunTrace(FHitResult& Hit, FVector& ShotDirection, const AController* PlayerController)
{

	FVector OutLocation;
	FRotator OutRotation;
	PlayerController->GetPlayerViewPoint(OutLocation, OutRotation);
	ShotDirection = -OutRotation.Vector();

	FVector End = OutLocation + OutRotation.Vector() * Range;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredActor(GetOwner());
	return GetWorld()->LineTraceSingleByChannel(Hit, OutLocation, End, ECollisionChannel::ECC_GameTraceChannel1, params);

}



AController* AGun::GetOwnerController() const
{
	APawn* PlayerPawn = Cast < APawn >(GetOwner());
	if (!PlayerPawn)
	{
		return NULL;
	}
	return PlayerPawn->GetController();
}


/*void AGun::EquipmentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp,Error , TEXT("EquipmentBeginOverlap"));
	AShooterCharacter* PlayerCharacter = Cast<AShooterCharacter>(OtherActor);
	if(PlayerCharacter)
	{
		FVector DistanceBetweenTwoActors = GetActorLocation() - OtherActor->GetActorLocation();
		int32 AngleOfOtherActorFace = acosf(FVector::DotProduct(DistanceBetweenTwoActors, OtherActor->GetActorForwardVector())) * (180 / 3.1415926);

		if(abs(AngleOfOtherActorFace) < 60)
		{
			UE_LOG(LogTemp,Warning , TEXT("Press E to grab"));
			
		}	
	}
}*/

void AGun::SetSimulateEnable(bool bEnable)
{
	PhysicsBox->SetSimulatePhysics(bEnable);
}



// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

