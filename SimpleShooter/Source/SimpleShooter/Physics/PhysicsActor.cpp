// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicsActor.h"

// Sets default values
APhysicsActor::APhysicsActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	PhysicsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Physics Box Component"));
	SetRootComponent(PhysicsBox);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Component"));
	CollisionBox->SetupAttachment(PhysicsBox);

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("scene Component"));
	SceneComponent->SetupAttachment(PhysicsBox);


}

// Called when the game starts or when spawned
void APhysicsActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APhysicsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float APhysicsActor::TakeDamage(float DamageAmount1, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	PhysicsBox->AddImpulse((GetActorLocation() - DamageCauser->GetActorLocation()).GetSafeNormal() * ImpulseForce);
	return 0;
}

