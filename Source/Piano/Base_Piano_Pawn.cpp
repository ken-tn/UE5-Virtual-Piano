// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_Piano_Pawn.h"
#include <fluidsynth.h>

#if defined(WIN32)
#include <windows.h>
#define sleep(_t) Sleep(_t * 1000)
#include <process.h>
#define getpid _getpid
#else
#include <stdlib.h>
#include <unistd.h>
#endif

int ABase_Piano_Pawn::LetterToNote(FKey Key)
{
	const FString letterNoteMap = "1!2@34$5%6^78*9(0qQwWeErtTyYuiIoOpPasSdDfgGhHjJklLzZxcCvVbBnm";
	const FString pressedLetter = Key.GetDisplayName().ToString().ToLower();

	int note = letterNoteMap.Find(pressedLetter);
	if (note == -1)
	{
		return note;
	}
	else
	{
		APlayerController* PlayerController = Cast<APlayerController>(ABase_Piano_Pawn::GetController());
		int offset = Transposition;
		if (PlayerController->IsInputKeyDown("LeftShift") || PlayerController->IsInputKeyDown("RightShift"))
		{
			offset += 1;
		}
		else if (PlayerController->IsInputKeyDown("LeftControl") || PlayerController->IsInputKeyDown("RightControl"))
		{
			offset -= 1;
		}

		if (PlayerController != nullptr)
		{
			return note + Transposition + offset;
		}
		else
		{
			return -1;
		}
	}
}

void ABase_Piano_Pawn::NoteOff(int note)
{
	UE_LOG(LogTemp, Warning, TEXT("Note Off: %d"), note);
	fluid_synth_noteoff(synth, 0, note);
}

void ABase_Piano_Pawn::OnKeyDown(FKey Key) {
	// Get note integer
	int note = LetterToNote(Key);
	UE_LOG(LogTemp, Warning, TEXT("%d"), note);
	if (note == -1)
	{
		return;
	}

	/* Play a note */
	fluid_synth_noteon(synth, 0, note, 80);
	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;

	//Binding the function with specific values
	TimerDel.BindUFunction(this, FName("NoteOff"), note);
	//Calling NoteOff after 1 seconds without looping
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1.f, false);
}

// Sets default values
ABase_Piano_Pawn::ABase_Piano_Pawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABase_Piano_Pawn::BeginPlay()
{
	Super::BeginPlay();
	
	fluid_settings_t* settings;
	fluid_audio_driver_t* adriver;
	int sfont_id;

	/* Create the settings. */
	settings = new_fluid_settings();

	/* Change the settings if necessary*/

	/* Create the synthesizer. */
	synth = new_fluid_synth(settings);

	/* Create the audio driver. The synthesizer starts playing as soon
	   as the driver is created. */
	adriver = new_fluid_audio_driver(settings, synth);

	/* Load a SoundFont and reset presets (so that new instruments
	 * get used from the SoundFont) */
	sfont_id = fluid_synth_sfload(synth, "D:/Documents/Unreal Projects/Piano/MasonHamlin-A-v7.sf2", 1);

	if (sfont_id == FLUID_FAILED)
	{
		UE_LOG(LogTemp, Warning, TEXT("Loading the SoundFont failed!"));
	}

	/* Initialize the random number generator */
	srand(getpid());
}

// Called every frame
void ABase_Piano_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABase_Piano_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("OnKeyDown", IE_Pressed, this, &ABase_Piano_Pawn::OnKeyDown);
}
