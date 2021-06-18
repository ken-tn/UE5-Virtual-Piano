// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_Piano_Pawn.h"
#include <fluidsynth.h>

int ABase_Piano_Pawn::LetterToNote(const FString KeyName)
{
	int note = letterNoteMap.Find(KeyName);

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
			return note + Transposition + offset + 36; // +36 :thonk:
		}
		else
		{
			return -1;
		}
	}
}

void ABase_Piano_Pawn::SetGain(const float newGain)
{
	fluid_synth_set_gain(vpsynth, newGain);
}

void ABase_Piano_Pawn::ToggleAuto()
{
	if (fluid_player_playing)
	{
		fluid_player_stop(fluid_player);
	}
	else
	{
		fluid_player_play(fluid_player);
	}

	fluid_player_playing = !fluid_player_playing;
}

void ABase_Piano_Pawn::OnKeyDown(FKey Key)
{
	const FString KeyName = *Key.GetDisplayName().ToString().ToLower();

	UE_LOG(LogTemp, Warning, TEXT("Key: %s"), *KeyName);

	if (KeyName == "up")
	{
		Transposition += 1;
		UE_LOG(LogTemp, Warning, TEXT("Transposition: %d"), Transposition);

		return;
	}
	else if (KeyName == "down")
	{
		Transposition -= 1;
		UE_LOG(LogTemp, Warning, TEXT("Transposition: %d"), Transposition);

		return;
	}
	else if (KeyName == "left")
	{
		Gain -= 0.01f;
		SetGain(Gain);
		UE_LOG(LogTemp, Warning, TEXT("Gain: %f"), Gain);

		return;
	}
	else if (KeyName == "right")
	{
		Gain += 0.01f;
		SetGain(Gain);
		UE_LOG(LogTemp, Warning, TEXT("Gain: %f"), Gain);

		return;
	}
	else if (KeyName == "delete")
	{
		ToggleAuto();
	}
	else if (KeyName == "space bar")
	{
		Sustain = !Sustain;
		if (Sustain)
		{
			fluid_synth_all_notes_off(vpsynth, 1);
		}
	}

	// Get note integer
	int note = LetterToNote(KeyName);
	if (note == -1)
	{
		return;
	}

	// Play note
	fluid_synth_noteon(vpsynth, 1, note, 127);
	UE_LOG(LogTemp, Warning, TEXT("%d"), note);
}

void ABase_Piano_Pawn::OnKeyUp(FKey Key)
{
	const FString KeyName = *Key.GetDisplayName().ToString().ToLower();
	if (KeyName == "space bar")
	{
		Sustain = !Sustain;
	}

	int note = LetterToNote(KeyName);
	if (note == -1 || !Sustain)
	{
		return;
	}

	// Release note
	fluid_synth_noteoff(vpsynth, 1, note);
	UE_LOG(LogTemp, Warning, TEXT("Key Released: %d"), note);
}

// Sets default values
ABase_Piano_Pawn::ABase_Piano_Pawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABase_Piano_Pawn::PrintAllInstruments(fluid_synth_t* synth, int sfont_id)
{
	fluid_preset_t* preset;

	fluid_sfont_t* sfont = fluid_synth_get_sfont_by_id(synth, sfont_id);
	int offset = fluid_synth_get_bank_offset(synth, sfont_id);
	fluid_sfont_iteration_start(sfont);

	while ((preset = fluid_sfont_iteration_next(sfont)) != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("%03d-%03d %s\n"),
			fluid_preset_get_banknum(preset) + offset,
			fluid_preset_get_num(preset),
			*FString(fluid_preset_get_name(preset)));
	}
}

// Called when the game starts or when spawned
void ABase_Piano_Pawn::BeginPlay()
{
	Super::BeginPlay();
	
	fluid_settings_t* settings;
	int sfont_id;

	settings = new_fluid_settings();
	vpsynth = new_fluid_synth(settings);
	midisynth = new_fluid_synth(settings);

	// Create the audio driver. The synthesizer starts playing as soon as the driver is created.
	fluid_audio_driver_t* adriver = new_fluid_audio_driver(settings, vpsynth);
	fluid_audio_driver_t* adriver2 = new_fluid_audio_driver(settings, midisynth);

	// Load a SoundFont and reset presets (so that new instruments get used from the SoundFont)
	const ANSICHAR* FontPath = TCHAR_TO_ANSI(*FPaths::ProjectContentDir().Append("Soundfonts/" + FontArray[PianoFont.GetValue()]));
	sfont_id = fluid_synth_sfload(vpsynth, FontPath, 1);
	fluid_synth_sfload(midisynth, FontPath, 1);

	if (sfont_id == FLUID_FAILED)
	{
		UE_LOG(LogTemp, Warning, TEXT("Loading the SoundFont failed!"));
	}

	fluid_synth_set_gain(vpsynth, Gain);

	// Create auto player
	fluid_player = new_fluid_player(midisynth);
	fluid_player_add(fluid_player, TCHAR_TO_ANSI(*FPaths::ProjectContentDir().Append("Midi/KOKORONASHI.mid")));

	// Print all instruments
	PrintAllInstruments(vpsynth, sfont_id);
}

void ABase_Piano_Pawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	fluid_player_stop(fluid_player);
	fluid_synth_all_notes_off(vpsynth, 1);
	fluid_synth_all_sounds_off(vpsynth, 1);
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

	PlayerInputComponent->BindAction("PianoKeyboardDown", IE_Pressed, this, &ABase_Piano_Pawn::OnKeyDown);
	PlayerInputComponent->BindAction("PianoKeyboardUp", IE_Released, this, &ABase_Piano_Pawn::OnKeyUp);
}
