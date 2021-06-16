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
	const FString letterNoteMap = "1!2@34$5%6^78*9(0qQwWeErtTyYuiIoOpPasSdDfgGhHjJklLzZxcCvVbBnm";
	fluid_synth_t* vpsynth;
	fluid_synth_t* midisynth;
	fluid_player_t* fluid_player;
	bool fluid_player_playing = false;

	void ToggleAuto();
	void OnKeyDown(FKey key);
	void OnKeyUp(FKey key);
	int LetterToNote(const FString KeyName);

	UPROPERTY(VisibleAnywhere)
		bool Sustain = false;

	UPROPERTY(VisibleAnywhere)
		float Gain = 0.1f;

	UPROPERTY(VisibleAnywhere)
		int Transposition = 0;

	UFUNCTION(BlueprintCallable)
		void SetGain(float NewGain);

	// Sets default values for this pawn's properties
	ABase_Piano_Pawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when game ends or destroyed
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
