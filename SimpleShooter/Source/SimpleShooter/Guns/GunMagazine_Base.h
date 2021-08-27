// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleShooter/Physics/PhysicsActor.h"
#include "GunMagazine_Base.generated.h"

UCLASS()
class SIMPLESHOOTER_API AGunMagazine_Base : public APhysicsActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGunMagazine_Base();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"));
	UStaticMeshComponent* Mesh;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Drop();

};
