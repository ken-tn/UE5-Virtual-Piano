// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <fluidsynth.h>
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Base_Piano_Pawn.generated.h"

UENUM()
enum ESoundFont {
	MasonHamlin		UMETA(DisplayName = "MasonHamlin A v7"),
	Arachno			UMETA(DisplayName = "Arachno SoundFont - Version 1.0"),
};

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
	TArray<FString> FontArray = {
		"MasonHamlin-A-v7.sf2",
		"Arachno SoundFont - Version 1.0.sf2"
	};

	void ToggleAuto();
	void OnKeyDown(FKey key);
	void OnKeyUp(FKey key);
	int LetterToNote(const FString KeyName);

	/** The type of object the interactable is */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Piano")
		TEnumAsByte<ESoundFont> PianoFont;

	UPROPERTY(VisibleAnywhere, Category = "Piano")
		bool Sustain = false;

	UPROPERTY(VisibleAnywhere, Category = "Piano")
		float Gain = 0.1f;

	UPROPERTY(VisibleAnywhere, Category = "Piano")
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
