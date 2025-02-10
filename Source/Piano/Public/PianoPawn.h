// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionsConfig.h"
#include <fluidsynth.h>
#include "W_PianoKey.h"
#include "PianoPawn.generated.h"

// Declare delegate types
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIntDelegate, const int, Int);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFloatDelegate, const float, Float);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStringIntDelegate, const FString, String, int, Int);

UCLASS()
class PIANO_API APianoPawn : public APawn
{
	GENERATED_BODY()

private:
#pragma region Properties
	TArray<UW_PianoKey*> UWPianoKeys;
	const FString letterNoteMap = "1!2@34$5%6^78*9(0qQwWeErtTyYuiIoOpPasSdDfgGhHjJklLzZxcCvVbBnm";
	fluid_synth_t* vpsynth;
	fluid_synth_t* midisynth;
	fluid_player_t* fluid_player;
	fluid_audio_driver_t* vpdriver;
	fluid_audio_driver_t* mididriver;
	int FontID = 1;
	int FontIndex = 0;
	int MidiIndex = 0;

	// Channel: Program
	int CurrentProgram[100] = { 0 };
#pragma endregion Properties

#pragma region Methods
	void ToggleAuto();
	void OnKeyDown(FKey key);
	void OnKeyUp(FKey key);
	void PrintAllInstruments(fluid_synth_t* synth, int sfont_id);
	int LetterToNote(const FString KeyName);
#pragma endregion Methods

public:
#pragma region Input
	/**
	* Set using a derived blueprint instance. The Input Mapping Context used for the pawn.
	*/
	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	/**
	* Set using a derived blueprint instance. The Input Config used for the pawn.
	*/
	UPROPERTY(EditAnywhere, Category = "Input")
	TSubclassOf<UInputActionsConfig> InputConfigData;

	// Trigger functions for each input action
	UFUNCTION(BlueprintCallable, Category = "Input", meta = (DisplayName = "UserInput"))
	void TriggerTransposeUp();

	UFUNCTION(BlueprintCallable, Category = "Input", meta = (DisplayName = "UserInput"))
	void TriggerTransposeDown();

	UFUNCTION(BlueprintCallable, Category = "Input", meta = (DisplayName = "UserInput"))
	void TriggerGainIncrementUp();

	UFUNCTION(BlueprintCallable, Category = "Input", meta = (DisplayName = "UserInput"))
	void TriggerGainIncrementDown();

	UFUNCTION(BlueprintCallable, Category = "Input", meta = (DisplayName = "UserInput"))
	void TriggerInstrumentNext();

	UFUNCTION(BlueprintCallable, Category = "Input", meta = (DisplayName = "UserInput"))
	void TriggerInstrumentPrevious();

	UFUNCTION(BlueprintCallable, Category = "Input", meta = (DisplayName = "UserInput"))
	void TriggerSoundfontNext();

	UFUNCTION(BlueprintCallable, Category = "Input", meta = (DisplayName = "UserInput"))
	void TriggerSoundfontPrevious();

	UFUNCTION(BlueprintCallable, Category = "Input", meta = (DisplayName = "UserInput"))
	void TriggerMIDINext();

	UFUNCTION(BlueprintCallable, Category = "Input", meta = (DisplayName = "UserInput"))
	void TriggerMIDIPrevious();

	UFUNCTION(BlueprintCallable, Category = "Input", meta = (DisplayName = "UserInput"))
	void TriggerAutoplayToggle();

	UFUNCTION(BlueprintCallable, Category = "Input", meta = (DisplayName = "UserInput"))
	void TriggerSustainPressed();

	UFUNCTION(BlueprintCallable, Category = "Input", meta = (DisplayName = "UserInput"))
	void TriggerSustainReleased();

#pragma endregion Input

#pragma region Properties
	// Properties
	UPROPERTY(VisibleAnywhere)
		TMap<int, FString> LoadedFonts;

	UPROPERTY(VisibleAnywhere, Category = "Piano")
		TArray<FString> Fonts = {};

	UPROPERTY(VisibleAnywhere, Category = "Piano")
		TArray<FString> Midis = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Piano")
		int DefaultFont = 2;

	UPROPERTY(VisibleAnywhere, Category = "Piano")
		bool Sustain = false;

	UPROPERTY(VisibleAnywhere, Category = "Piano")
		float Gain = 0.1f;

	UPROPERTY(VisibleAnywhere, Category = "Piano")
		int Transposition = 0;
#pragma endregion Properties

#pragma region Methods
	void Initialize();

	UFUNCTION()
		int LoadSoundfont(int fontIndex);
#pragma endregion Methods

#pragma region BPMethods
	// Functions
	UFUNCTION(BlueprintCallable, Category = "Piano")
		void TransposeIncrement(int Increment);

	UFUNCTION(BlueprintCallable, Category = "Piano")
		void GainIncrement(float Increment);

	UFUNCTION(BlueprintCallable, Category = "Piano")
		int ChangeInstrument(int fontid, int programindex);

	UFUNCTION(BlueprintCallable, Category = "Piano")
		void InstrumentIncrement(int Increment);

	UFUNCTION(BlueprintCallable, Category = "Piano")
		void SoundFontIncrement(int Increment);

	UFUNCTION(BlueprintCallable, Category = "Piano")
		void MidiIncrement(int Increment);

	UFUNCTION(BlueprintCallable, Category = "Piano")
		void SetCurrentNote(float note);
#pragma endregion BPMethods

#pragma region Delegates
	// Delegates
	UPROPERTY(BlueprintAssignable)
	FIntDelegate TransposeChanged;

	UPROPERTY(BlueprintAssignable)
	FFloatDelegate GainChanged;

	UPROPERTY(BlueprintAssignable)
	FStringIntDelegate InstrumentChanged;

	UPROPERTY(BlueprintAssignable)
	FStringIntDelegate FontChanged;

	UPROPERTY(BlueprintAssignable)
	FStringIntDelegate MidiChanged;

#pragma endregion Delegates

	/** Property replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Sets default values for this pawn's properties
	APianoPawn();

protected:
	UPROPERTY(EditAnywhere, Category = "Class Types")
	TSubclassOf<UUserWidget> WidgetClass;

	class UW_Piano* PianoWidget;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CurrentNote, Category = "Piano")
		int CurrentNote = 1;

	/** RepNotify for changes made to current health.*/
	UFUNCTION()
		void OnRep_CurrentNote();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when game ends or destroyed
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Response to note being played. Called on the server immediately after modification, and on clients in response to a RepNotify*/
	void OnNotePlayed();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
