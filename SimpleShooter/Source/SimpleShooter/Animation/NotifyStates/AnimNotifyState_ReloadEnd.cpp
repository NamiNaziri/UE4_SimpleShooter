// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_ReloadEnd.h"
#include "SimpleShooter/Player/Character/ShooterCharacter.h"
#include "SimpleShooter/Guns/Gun.h"
#include "SimpleShooter/Guns/GunMagazine_Base.h"


void UAnimNotifyState_ReloadEnd::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration)
{

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

void UAnimNotifyState_ReloadEnd::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AShooterCharacter* PlayerCharacter = Cast<AShooterCharacter>(MeshComp->GetOwner());
	if (PlayerCharacter)
	{
		AGun* PlayerGun = PlayerCharacter->GetCurrentWeaponRefrence();
		if (PlayerGun)
		{
			USkeletalMeshComponent* GunMesh = PlayerGun->GetMesh();

			GunMesh->UnHideBoneByName(BoneNameToUnHide);
			
			if(newMag)
			{
				newMag->Destroy();
			}


		}
	}
}

