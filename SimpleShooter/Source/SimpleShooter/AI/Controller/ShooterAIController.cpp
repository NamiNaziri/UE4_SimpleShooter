// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "behaviortree/BlackboardComponent.h"
#include "SimpleShooter/Player/Character/ShooterCharacter.h"
void AShooterAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIBehavior)
	{
		RunBehaviorTree(AIBehavior);
		PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
	}
	
}


void AShooterAIController::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);


}

bool AShooterAIController::IsDead()
{
	AShooterCharacter* AICharacter = Cast<AShooterCharacter> (this->GetPawn());
	if (AICharacter)
	{
		return AICharacter->IsDead();
	}

	return true;
}
