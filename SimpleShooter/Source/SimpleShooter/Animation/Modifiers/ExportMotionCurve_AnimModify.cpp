// Fill out your copyright notice in the Description page of Project Settings.


#include "ExportMotionCurve_AnimModify.h"
#include "AnimationBlueprintLibrary.h"
void UExportMotionCurve_AnimModify::OnApply_Implementation(UAnimSequence* AnimationSequence)
{
	Super::OnApply_Implementation(AnimationSequence);

	FTransform RootTransform;

	UAnimationBlueprintLibrary::AddAnimationNotifyTrack(AnimationSequence, "MotionTrack");

	UAnimationBlueprintLibrary::AddCurve(AnimationSequence, "LocationCurveX");
	UAnimationBlueprintLibrary::AddCurve(AnimationSequence, "LocationCurveY");
	UAnimationBlueprintLibrary::AddCurve(AnimationSequence, "RotationCurve");



	float LocationDeltaZeroX = 0;
	float LocationDeltaZeroY = 0;
	float RotationDeltaZero = 0;
	for(int i = 0 ; i < AnimationSequence->GetNumberOfFrames() ; i++)
	{

		UAnimationBlueprintLibrary::GetBonePoseForFrame(AnimationSequence, "root", i, true, RootTransform);
		const FVector RootLocation = RootTransform.GetLocation();
		const FRotator RootRotation = RootTransform.GetRotation().Rotator();

		if (i == 0)
		{
			LocationDeltaZeroX = -1 * RootLocation.X;
			LocationDeltaZeroY = -1 * RootLocation.Y;
			RotationDeltaZero = -1 * RootRotation.Yaw;
			
		}

		float CurveKeyTime;
		UAnimationBlueprintLibrary::GetTimeAtFrame(AnimationSequence, i, CurveKeyTime);
		UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, "LocationCurveX", CurveKeyTime, RootLocation.X + LocationDeltaZeroX);

		UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, "LocationCurveY",CurveKeyTime,RootLocation.Y + LocationDeltaZeroY);
		UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, "RotationCurve", CurveKeyTime, RootRotation.Yaw + RotationDeltaZero);

	}

	
	
}
