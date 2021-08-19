// Fill out your copyright notice in the Description page of Project Settings.


#include "KillEmAllGameMode.h"
#include "EngineUtils.h"
#include "GameFramework/Controller.h"
#include "SimpleShooter/AI/Controller/ShooterAIController.h"
void AKillEmAllGameMode::PawnKilled(APawn* KilledPawn)
{
	APlayerController* KilledController = Cast<APlayerController>(KilledPawn->GetController());
	if (KilledController)
	{
		EndGame(false);
		return;
	}

	bool AllAIsDead = true;
	
	for (AShooterAIController* Controller : TActorRange<AShooterAIController>(GetWorld()))
	{
		if (!Controller->IsDead())
		{
			return;
		}
	}
	EndGame(true);
	

}

void AKillEmAllGameMode::EndGame(bool IsPlayerWinner)
{

	for (AController* Controller : TActorRange<AController>(GetWorld()))
	{
		bool bIsWinner = Controller->IsPlayerController() == IsPlayerWinner;
		Controller->GameHasEnded(Controller->GetPawn(), bIsWinner);

	}

	
	
}
