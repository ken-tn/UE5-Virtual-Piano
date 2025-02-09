// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PianoPawn.h"
#include "PianoGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class PIANO_API APianoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	/**
	* Set using a derived blueprint instance. The pawn used to interact with the game.
	*/
	UPROPERTY(EditAnywhere)
	TSubclassOf<APianoPawn> ControllerPawn;

protected:
	UPROPERTY(EditAnywhere, Category = "Class Types")
		TSubclassOf<UUserWidget> WidgetClass;

	virtual void PostLogin(APlayerController* NewPlayer) override;
};
