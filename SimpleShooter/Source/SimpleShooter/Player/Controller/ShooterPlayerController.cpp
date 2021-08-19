// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
void AShooterPlayerController::GameHasEnded(class AActor* EndGameFocus /* = nullptr */, bool bIsWinner /* = false */)
{
	Super::GameHasEnded(EndGameFocus,bIsWinner);

	UUserWidget* LooseScreen = CreateWidget(this, LooseScreenClass);
	UUserWidget* WinScreen = CreateWidget(this, WinScreenClass);
	if (bIsWinner)
	{
		if (WinScreen)
		{
			WinScreen->AddToViewport();
		}
	}
	else
	{
		if (LooseScreen)
		{
			if (HudWidget)
			{
				HudWidget->RemoveFromViewport();
			}
			LooseScreen->AddToViewport();
		}
	}
	
	GetWorldTimerManager().SetTimer(TimerHandle, this, &APlayerController::RestartLevel, ResetTimeDelay);

	UE_LOG(LogTemp, Warning, TEXT("Game Has Ended"));
}



void AShooterPlayerController::BeginPlay()
{
	 HudWidget = CreateWidget(this, HudClass);
	if (HudWidget)
	{
		HudWidget->AddToViewport();
	}
}
