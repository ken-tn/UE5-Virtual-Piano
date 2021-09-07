// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PianoGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class PIANO_API APianoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Class Types")
		TSubclassOf<UUserWidget> WidgetClass;

	virtual void PostLogin(APlayerController* NewPlayer) override;
};
