// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "ExportMotionCurve_AnimModify.generated.h"



/**
 * 
 */
UCLASS()
class SIMPLESHOOTER_API UExportMotionCurve_AnimModify : public UAnimationModifier
{
	GENERATED_BODY()


public:

	virtual void OnApply_Implementation(UAnimSequence* AnimationSequence) override;

	
};
