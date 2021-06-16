// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <fluidsynth.h>
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Base_Piano_Pawn.generated.h"

UCLASS()
class PIANO_API ABase_Piano_Pawn : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		int Transposition = 0;

	int LetterToNote(FKey Key);
	void OnKeyDown(FKey key);

	fluid_synth_t* synth;

	UFUNCTION()
	void NoteOff(int note);

	// Sets default values for this pawn's properties
	ABase_Piano_Pawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
