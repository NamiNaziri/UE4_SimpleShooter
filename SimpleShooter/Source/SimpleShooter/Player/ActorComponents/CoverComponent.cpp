// Fill out your copyright notice in the Description page of Project Settings.


#include "CoverComponent.h"
#include "DrawDebugHelpers.h"
// Sets default values for this component's properties
UCoverComponent::UCoverComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	MaximumCoverRay = CreateDefaultSubobject<USceneComponent>(TEXT("Maximum Cover Ray"));
	MinimumCoverRay = CreateDefaultSubobject<USceneComponent>(TEXT("Minimum Cover Ray"));

	RightRay = CreateDefaultSubobject<USceneComponent>(TEXT("Right Cover Ray"));
	LeftRay = CreateDefaultSubobject<USceneComponent>(TEXT("Left Cover Ray"));
	

	MaximumCoverRay->SetupAttachment(this);
	MinimumCoverRay->SetupAttachment(this);
	RightRay->SetupAttachment(this);
	LeftRay->SetupAttachment(this);

}

FCoverQuery UCoverComponent::EnvironmentalQuery()
{
	FCoverQuery OutQuery;
	if(!GetWorld()->GetFirstPlayerController())
	{
		return OutQuery;
	}
	FRotator ControllerRotation = GetWorld()->GetFirstPlayerController()->GetControlRotation();
	ControllerRotation.Roll = 0;
	ControllerRotation.Pitch = 0;
	const float Yaw = ControllerRotation.Yaw;

	const FVector Direction = ControllerRotation.Vector();
	
	
	AActor* Owner = GetOwner();
	FVector Start;
	FVector End;
	
	
	Start = MinimumCoverRay->GetRelativeLocation().RotateAngleAxis(Yaw, GetOwner()->GetActorUpVector()) + Owner->GetActorLocation() ;;
	End = Start + Direction * AcceptableDistanceToCover;

	FHitResult MinQueryHit;
	FCollisionQueryParams Minparams;
	Minparams.AddIgnoredActor(GetOwner());
	OutQuery.bHitMin = GetWorld()->LineTraceSingleByChannel(MinQueryHit, Start, End, ECollisionChannel::ECC_GameTraceChannel1, Minparams);
	if(OutQuery.bHitMin)
	{
		OutQuery.HitMin = MinQueryHit;
		if(GetWorld())
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Red);
		}
	}

	///////////////
	Start = MaximumCoverRay->GetRelativeLocation().RotateAngleAxis(Yaw, GetOwner()->GetActorUpVector()) + Owner->GetActorLocation();;
	End = Start + Direction * AcceptableDistanceToCover;

	FHitResult MaxQueryHit;
	FCollisionQueryParams MaxParams;
	MaxParams.AddIgnoredActor(GetOwner());
	OutQuery.bHitMAx = GetWorld()->LineTraceSingleByChannel(MaxQueryHit, Start, End, ECollisionChannel::ECC_GameTraceChannel1, MaxParams);
	if (OutQuery.bHitMAx)
	{
		OutQuery.HitMAx = MaxQueryHit;
		if (GetWorld())
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Red);
		}
	}

	///////////////////////

	Start = RightRay->GetRelativeLocation().RotateAngleAxis(Yaw, GetOwner()->GetActorUpVector()) + Owner->GetActorLocation();
	End = Start + Direction * AcceptableDistanceToCover;

	FHitResult RightQueryHit;
	FCollisionQueryParams RightParams;
	RightParams.AddIgnoredActor(GetOwner());
	OutQuery.bHitRight = GetWorld()->LineTraceSingleByChannel(RightQueryHit, Start, End, ECollisionChannel::ECC_GameTraceChannel1, RightParams);
	if (OutQuery.bHitRight)
	{
		OutQuery.HitRight = RightQueryHit;
		if (GetWorld())
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Red);
		}
	}


	////////////////////////
	Start = LeftRay->GetRelativeLocation().RotateAngleAxis(Yaw, GetOwner()->GetActorUpVector()) + Owner->GetActorLocation();;
	End = Start + Direction * AcceptableDistanceToCover;

	FHitResult LeftQueryHit;
	FCollisionQueryParams LeftParams;
	LeftParams.AddIgnoredActor(GetOwner());
	OutQuery.bHitLeft = GetWorld()->LineTraceSingleByChannel(LeftQueryHit, Start, End, ECollisionChannel::ECC_GameTraceChannel1, LeftParams);
	if (OutQuery.bHitLeft)
	{
		OutQuery.HitLeft = LeftQueryHit;
		if (GetWorld())
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Red);
		}
	}

	return OutQuery;
	
}


void UCoverComponent::ContructionScript()
{
	if (!GetWorld()->GetFirstPlayerController())
	{
		return;
	}
	const float Yaw = GetWorld()->GetFirstPlayerController()->GetControlRotation().Yaw;
	//const float Yaw = GetOwner()->GetActorRotation().Yaw;
	
	DrawDebugCoordinateSystem(GetWorld(), GetOwner()->GetActorLocation() + MinimumCoverRay->GetRelativeLocation().RotateAngleAxis(Yaw,GetOwner()->GetActorUpVector())  , MinimumCoverRay->GetRelativeRotation() , 5, false, GetWorld()->GetDeltaSeconds(), 0, 2);
	DrawDebugCoordinateSystem(GetWorld(), GetOwner()->GetActorLocation() + RightRay->GetRelativeLocation().RotateAngleAxis(Yaw, GetOwner()->GetActorUpVector()), RightRay->GetRelativeRotation(), 5, false, GetWorld()->GetDeltaSeconds(), 0, 2);
	DrawDebugCoordinateSystem(GetWorld(), GetOwner()->GetActorLocation() + LeftRay->GetRelativeLocation().RotateAngleAxis(Yaw, GetOwner()->GetActorUpVector()), LeftRay->GetRelativeRotation(), 5, false, GetWorld()->GetDeltaSeconds(), 0, 2);

	DrawDebugCoordinateSystem(GetWorld(), GetOwner()->GetActorLocation() + MaximumCoverRay->GetRelativeLocation().RotateAngleAxis(Yaw, GetOwner()->GetActorUpVector()), MaximumCoverRay->GetRelativeRotation(), 5, false, GetWorld()->GetDeltaSeconds(),0,2);
}

// Called when the game starts
void UCoverComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCoverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

