// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_ReloadStart.h"

#include "SimpleShooter/Player/Character/ShooterCharacter.h"
#include "SimpleShooter/Guns/Gun.h"
#include "SimpleShooter/Guns/GunMagazine_Base.h"


void UAnimNotifyState_ReloadStart::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration)
{

	AShooterCharacter* PlayerCharacter = Cast<AShooterCharacter>(MeshComp->GetOwner());
	if (PlayerCharacter)
	{
		AGun* PlayerGun = PlayerCharacter->GetCurrentWeaponRefrence();
		if (PlayerGun)
		{
			USkeletalMeshComponent* GunMesh = PlayerGun->GetMesh();
			GunMesh->HideBoneByName(BoneNameToHide, EPhysBodyOp::PBO_None);

			FActorSpawnParameters SpawnInfo;
			SpawnInfo.ObjectFlags = RF_NoFlags;

			FVector Location;
			FRotator Rotation;

			if (GunMagClass)
			{
				if (MeshComp->GetWorld())
				{
					newMag = MeshComp->GetWorld()->SpawnActor<AGunMagazine_Base>(GunMagClass);
					if (newMag)
					{
						newMag->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, "MagSocket");
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("World Not found in animNotify_ReloadStart"));
				}

			}


		}
	}
}

void UAnimNotifyState_ReloadStart::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if(newMag)
	{
		newMag->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		newMag->Drop();
	}
}
