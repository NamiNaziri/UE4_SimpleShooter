// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SimpleShooter/Enums/EPlayerCoverState.h"
#include "CoverComponent.generated.h"




USTRUCT(BlueprintType)
struct FCoverQuery
{
	GENERATED_BODY()

	//~ The following member variable will be accessible by Blueprint Graphs:
	// This is the tooltip for our test variable.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Query")
	EPlayerCoverState PlayerCoverState;

	UPROPERTY(BlueprintReadOnly, Category = "Cover Query")
	bool bHitRight;
	UPROPERTY(BlueprintReadOnly, Category = "Cover Query")
	bool bHitLeft;
	UPROPERTY(BlueprintReadOnly, Category = "Cover Query")
	bool bHitMAx;
	UPROPERTY(BlueprintReadOnly, Category = "Cover Query")
	bool bHitMin;

	UPROPERTY(BlueprintReadOnly, Category = "Cover Query")
	FHitResult HitRight;
	UPROPERTY(BlueprintReadOnly, Category = "Cover Query")
	FHitResult HitLeft;
	UPROPERTY(BlueprintReadOnly, Category = "Cover Query")
	FHitResult HitMAx;
	UPROPERTY(BlueprintReadOnly, Category = "Cover Query")
	FHitResult HitMin;
	
	
};



UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLESHOOTER_API UCoverComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCoverComponent();

	UFUNCTION(BlueprintCallable)
	FCoverQuery EnvironmentalQuery();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
	float CoverSurfaceDistanceCheck = 15.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
	float CoverSurfaceRayNumbers = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
	float AcceptableDistanceToCover = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
		USceneComponent* MinimumCoverRay;
	UPROPERTY(EditAnywhere, Category = "Cover")
		USceneComponent* MaximumCoverRay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
		USceneComponent* RightRay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
		USceneComponent* LeftRay;


	UFUNCTION(BlueprintCallable)
		void ContructionScript();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
