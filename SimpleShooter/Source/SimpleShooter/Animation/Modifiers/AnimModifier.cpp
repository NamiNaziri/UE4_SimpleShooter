// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimModifier.h"
#include "AnimationBlueprintLibrary.h" 
void UAnimModifier::OnApply_Implementation(UAnimSequence* AnimationSequence)
{

	UE_LOG(LogTemp,Warning,TEXT("HI"));

	UAnimationBlueprintLibrary::AddAnimationNotifyTrack(AnimationSequence,"FootSyncNotify");

	int NumOfFrames = AnimationSequence->GetNumberOfFrames();
	for(int i = 0 ; i < NumOfFrames ; i++)
	{
		
		TransformR = FTransform::Identity;
		TransformL = FTransform::Identity;
		PrevTransformR = TransformR;
		PrevTransformL = TransformL;

		FindWorldTransform(TransformR,"Foot_R",i,AnimationSequence);
		FindWorldTransform(TransformL,"Foot_L",i,AnimationSequence);

		if(i != 0)
		{
			AddAnimSyncMarker(AnimationSequence,"RightFoot",i,TransformR,PrevTransformR,TransformL);
			AddAnimSyncMarker(AnimationSequence,"LeftFoot",i,TransformL,PrevTransformL,TransformR);
		}
		else
		{
			FlagDoOnce = true;
		}
		
	}
	
}

void UAnimModifier::FindWorldTransform(FTransform& OutTransform, FName BoneName, int Index,
	UAnimSequence* AnimationSequence)
{
	TArray<FName> BonePath;
	UAnimationBlueprintLibrary::FindBonePathToRoot(AnimationSequence,BoneName,BonePath);

	for (auto Path : BonePath)
	{
		if(Path == "root")
		{
			break;
		}
		FTransform BonePoseForFrame;
		
		UAnimationBlueprintLibrary::GetBonePoseForFrame(AnimationSequence,Path,Index,false,BonePoseForFrame);

		OutTransform = OutTransform * BonePoseForFrame;
		
	}
	
	
}

void UAnimModifier::AddAnimSyncMarker(UAnimSequence* AnimationSequence, FName MarkerName,int32 Frame, FTransform CurrentTransform, FTransform PrevTransForm, FTransform OtherTransform)
{

	

	float CurrY = CurrentTransform.GetTranslation().Y;
	float PrevY = PrevTransForm.GetTranslation().Y;

	if(CurrY * PrevY < 0)
	{
		if( CurrentTransform.GetTranslation().Z < OtherTransform.GetTranslation().Z )
		{
			if(FlagDoOnce)
			{
				FlagDoOnce = false;

				
				if(!(abs(CurrY) < abs(PrevY)))
				{
					Frame = Frame - 1;
				}

				float time;
				UAnimationBlueprintLibrary::GetTimeAtFrame(AnimationSequence,Frame,time);

				
				UE_LOG(LogTemp,Warning,TEXT("H2I"));
				UAnimationBlueprintLibrary::AddAnimationSyncMarker(AnimationSequence,MarkerName,time,"FootSyncNotify");

				
			}
			
			
		}
		else
		{
			// flagdoone = true ???
		}
	}
	else
	{
		FlagDoOnce = true;
		
	}
	
}
