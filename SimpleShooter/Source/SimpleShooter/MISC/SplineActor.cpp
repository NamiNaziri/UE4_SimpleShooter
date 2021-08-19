// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineActor.h"

// Sets default values
ASplineActor::ASplineActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	if(SplineComponent)
	{
		SetRootComponent(SplineComponent);
	}
	
}

void ASplineActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	
}

void ASplineActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}





