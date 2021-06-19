// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <fluidsynth.h>
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Base_Piano_Pawn.generated.h"

// Declare delegate types
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIntDelegate, const int, Int);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFloatDelegate, const float, Float);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStringIntDelegate, const FString, String, int, Int);

UCLASS()
class PIANO_API ABase_Piano_Pawn : public APawn
{
	GENERATED_BODY()

public:
	const FString letterNoteMap = "1!2@34$5%6^78*9(0qQwWeErtTyYuiIoOpPasSdDfgGhHjJklLzZxcCvVbBnm";
	fluid_synth_t* vpsynth;
	fluid_synth_t* midisynth;
	fluid_player_t* fluid_player;
	int Channel = 1;
	bool fluid_player_playing = false;

	// Channel: Program
	int CurrentProgram[100] = { 0 };

	void ToggleAuto();
	void OnKeyDown(FKey key);
	void OnKeyUp(FKey key);
	void PrintAllInstruments(fluid_synth_t* synth, int sfont_id);
	void Initialize();
	void OnEndPlay();
	int LetterToNote(const FString KeyName);

	// Delegates
	UPROPERTY(BlueprintAssignable)
		FIntDelegate TransposeChanged;

	UPROPERTY(BlueprintAssignable)
		FFloatDelegate GainChanged;

	UPROPERTY(BlueprintAssignable)
		FStringIntDelegate InstrumentChanged;

	UPROPERTY(BlueprintAssignable)
		FStringIntDelegate FontChanged;

	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Piano")
		FString DefaultFont = "MasonHamlin-A-v7.sf2";

	UPROPERTY(VisibleAnywhere, Category = "Piano")
		TArray<FString> FontArray = {};

	UPROPERTY(VisibleAnywhere, Category = "Piano")
		bool Sustain = false;

	UPROPERTY(VisibleAnywhere, Category = "Piano")
		float Gain = 0.1f;

	UPROPERTY(VisibleAnywhere, Category = "Piano")
		int Transposition = 0;

	// Functions
	UFUNCTION(BlueprintCallable)
		void TransposeIncrement(int Increment);

	UFUNCTION(BlueprintCallable)
		void GainIncrement(float Increment);

	UFUNCTION(BlueprintCallable)
		FString ChangeInstrument(const int chanindex, const int programindex);

	UFUNCTION(BlueprintCallable)
		void InstrumentIncrement(int Increment);

	UFUNCTION(BlueprintCallable)
		void SoundFontIncrement(int Increment);

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
