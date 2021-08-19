// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/SplineComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "SplineActor.generated.h"


UENUM(BlueprintType)
enum class ESplineMeshType: uint8
{
	DEFAULT		UMETA(DisplayName = "Default Mesh"),
    START 		UMETA(DisplayName = "Start Mesh"),
    END 		UMETA(DisplayName = "End Mesh")
};


UCLASS()


class SIMPLESHOOTER_API ASplineActor : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere,Category = "Spline");
	USplineComponent* SplineComponent;
	UPROPERTY(VisibleAnywhere,Category = "Spline");
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent;
	
	
public:	
	// Sets default values for this actor's properties
	ASplineActor();
	virtual void OnConstruction(const FTransform& Transform) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
