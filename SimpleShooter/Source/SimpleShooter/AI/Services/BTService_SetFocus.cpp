// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetFocus.h"
#include "SimpleShooter/AI/Controller/ShooterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
UBTService_SetFocus::UBTService_SetFocus()
{
	NodeName = "SetFocus";
}

void UBTService_SetFocus::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	OwnerComp.GetAIOwner()->SetFocus(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}
