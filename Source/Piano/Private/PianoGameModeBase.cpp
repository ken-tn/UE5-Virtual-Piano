// Copyright Epic Games, Inc. All Rights Reserved.

#include "PianoGameModeBase.h"
#include "Engine/World.h"
#include "PianoPawn.h"

void APianoGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	if (ControllerPawn)
	{
		FActorSpawnParameters SpawnInfo;
		APianoPawn* Pawn = GetWorld()->SpawnActor<APianoPawn>(ControllerPawn, FVector(), FRotator(), SpawnInfo);
		NewPlayer->Possess(Pawn);
		Pawn->Initialize();
	}

	return;
}
