// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_ReloadEnd.generated.h"

class AGunMagazine_Base;
/**
 * 
 */
UCLASS()
class SIMPLESHOOTER_API UAnimNotifyState_ReloadEnd : public UAnimNotifyState
{
	GENERATED_BODY()
public:


	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	UPROPERTY(EditAnywhere, Category = Gun)
		FName BoneNameToUnHide = FName(TEXT("b_gun_mag"));
	UPROPERTY(EditAnywhere, Category = Gun)
		TSubclassOf<AGunMagazine_Base> GunMagClass;

private:
	UPROPERTY()
		AGunMagazine_Base* newMag;
};
