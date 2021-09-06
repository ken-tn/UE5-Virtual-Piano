// Copyright Epic Games, Inc. All Rights Reserved.


#include "PianoGameModeBase.h"
#include "Engine/World.h"
#include "Base_Piano_Pawn.h"
#include "W_Piano.h"

void APianoGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	FActorSpawnParameters SpawnInfo;
	ABase_Piano_Pawn* Pawn = GetWorld()->SpawnActor<ABase_Piano_Pawn>(FVector(), FRotator(), SpawnInfo);
	NewPlayer->Possess(Pawn);
	Pawn->Initialize();

	return;
}
