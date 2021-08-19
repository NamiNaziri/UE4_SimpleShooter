// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskShoot.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "SimpleShooter/Player/Character/ShooterCharacter.h"

UBTTaskShoot::UBTTaskShoot()
{
	NodeName = TEXT("Shoot");
}

EBTNodeResult::Type UBTTaskShoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(AIController->GetPawn());
	if (!ShooterCharacter)
	{
		return EBTNodeResult::Failed;
	}

	ShooterCharacter->Shoot();

	return EBTNodeResult::Succeeded;
}
