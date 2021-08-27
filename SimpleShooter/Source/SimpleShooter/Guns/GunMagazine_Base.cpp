// Fill out your copyright notice in the Description page of Project Settings.


#include "GunMagazine_Base.h"

// Sets default values
AGunMagazine_Base::AGunMagazine_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gun Mesh Component"));
	Mesh->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void AGunMagazine_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGunMagazine_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGunMagazine_Base::Drop()
{
	Super::PhysicsBox->SetSimulatePhysics(true);
}

