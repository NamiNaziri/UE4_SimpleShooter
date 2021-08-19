// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateDistanceToPlayer.h"
#include "SimpleShooter/AI/Controller/ShooterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/pawn.h"
UBTService_UpdateDistanceToPlayer::UBTService_UpdateDistanceToPlayer()
{
	NodeName = "Update Distance To Player";
}

void UBTService_UpdateDistanceToPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);


	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* AIPawn = AIController->GetPawn();
	if ((!AIController) || (!PlayerPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("No AIController or PlayerPawn"));
		return;
	}


	float distance = FVector::Dist(OwnerComp.GetBlackboardComponent()->GetValueAsVector(TEXT("LastKnownPlayerLocation")), AIPawn->GetActorLocation());
	
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(GetSelectedBlackboardKey(), distance);


}
