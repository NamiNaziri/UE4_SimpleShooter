// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "AnimModifier.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLESHOOTER_API UAnimModifier : public UAnimationModifier
{
	GENERATED_BODY()

	public:


	virtual void OnApply_Implementation(UAnimSequence* AnimationSequence) override;

	private:

	FTransform TransformR, TransformL, PrevTransformR, PrevTransformL;
	UPROPERTY(EditAnywhere)
	bool FlagDoOnce;

	void FindWorldTransform(FTransform& OutTransform, FName BoneName, int Index, UAnimSequence* AnimationSequence);
	void AddAnimSyncMarker(UAnimSequence* AnimationSequence, FName MarkerName,int32 Frame, FTransform CurrentTransform, FTransform PrevTransForm, FTransform OtherTransform);
	
};
