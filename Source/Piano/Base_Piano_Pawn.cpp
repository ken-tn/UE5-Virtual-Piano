// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_Piano_Pawn.h"
#include <fluidsynth.h>
#include "HAL/FileManagerGeneric.h"

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

void ABase_Piano_Pawn::TransposeIncrement(int Increment)
{
	Transposition += Increment;
	TransposeChanged.Broadcast(Transposition);
	UE_LOG(LogTemp, Warning, TEXT("Transposition: %d"), Transposition);
}

void ABase_Piano_Pawn::GainIncrement(float Increment)
{
	Gain += Increment;
	fluid_synth_set_gain(vpsynth, Gain);
	GainChanged.Broadcast(Gain);
	UE_LOG(LogTemp, Warning, TEXT("Gain: %f"), Gain);
}

FString ABase_Piano_Pawn::ChangeInstrument(const int chanindex, const int programindex)
{
	if (programindex < 0)
	{
		return "";
	}
	fluid_sfont_t* sfont = fluid_synth_get_sfont_by_id(vpsynth, Channel);
	fluid_preset_t* preset;

	fluid_sfont_iteration_start(sfont);
	for (int i = 0; i < programindex; i++)
	{
		preset = fluid_sfont_iteration_next(sfont);
		if (preset == NULL)
		{
			return "";
		}
	}

	if ((preset = fluid_sfont_iteration_next(sfont)) != NULL)
	{
		fluid_synth_program_change(vpsynth, chanindex, programindex);
		fluid_synth_program_change(midisynth, chanindex, programindex);
		return FString(fluid_preset_get_name(preset));
	}
	else
	{
		return "";
	}
}

void ABase_Piano_Pawn::InstrumentIncrement(int Increment)
{
	CurrentProgram[Channel - 1] += Increment;
	FString NewInstrument = ChangeInstrument(Channel, CurrentProgram[Channel - 1]);
	if (NewInstrument == "")
	{
		CurrentProgram[Channel - 1] -= Increment;
	}
	else
	{
		InstrumentChanged.Broadcast(NewInstrument, CurrentProgram[Channel - 1]);
		UE_LOG(LogTemp, Warning, TEXT("Preset: %s"), *NewInstrument);
	}
}

void ABase_Piano_Pawn::SoundFontIncrement(int Increment)
{
	Channel += Increment;
	if (Channel > Fonts.Num() || Channel < 1)
	{
		Channel -= Increment;
		return;
	}
	const FString fontFileName = *Fonts[Channel - 1];

	FontChanged.Broadcast(fontFileName, Channel);
	UE_LOG(LogTemp, Warning, TEXT("Sound Font: %s"), *fontFileName);
	InstrumentIncrement(0);
}

void ABase_Piano_Pawn::MidiIncrement(int Increment)
{
	MidiIndex += Increment;
	if (MidiIndex > Midis.Num() || MidiIndex < 0)
	{
		MidiIndex -= Increment;
		return;
	}

	delete_fluid_player(fluid_player);
	// Create auto player
	const FString midiFileName = Midis[MidiIndex];
	fluid_player = new_fluid_player(midisynth);
	fluid_player_add(fluid_player, TCHAR_TO_ANSI(*FPaths::ProjectContentDir().Append("Midi/" + midiFileName)));
	if (fluid_player_playing)
	{
		fluid_player_play(fluid_player);
	}
	UE_LOG(LogTemp, Warning, TEXT("Midi track: %s"), *midiFileName);
}

void ABase_Piano_Pawn::OnKeyDown(FKey Key)
{
	const FString KeyName = *Key.GetDisplayName().ToString().ToLower();

	// print key
	UE_LOG(LogTemp, Warning, TEXT("Key: %s"), *KeyName);

	/*
	* pointers are scary
	#define FUNC_2(f, p1, p2) ((mathfunc2)(f))(p1, p2)
	#define FUNC_4(f, p1, p2, p3, p4) ((mathfunc4)(f))(p1, p2, p3, p4)
	typedef int ABase_Piano_Pawn::* (ABase_Piano_Pawn::*generic_fp)(void);

	TMap<FString, generic_fp> funcMap;
	funcMap.Add("up", ( (generic_fp)(&ABase_Piano_Pawn::TransposeIncrement) ));
	*/
	if (KeyName == "up")
	{
		TransposeIncrement(1);
		return;
	}
	else if (KeyName == "down")
	{
		TransposeIncrement(-1);
		return;
	}
	else if (KeyName == "left")
	{
		GainIncrement(-0.01f);
		return;
	}
	else if (KeyName == "right")
	{
		GainIncrement(0.01f);
		return;
	}
	else if (KeyName == "home")
	{
		InstrumentIncrement(1);
		return;
	}
	else if (KeyName == "end")
	{
		InstrumentIncrement(-1);
		return;
	}
	else if (KeyName == "right bracket")
	{
		SoundFontIncrement(1);
	}
	else if (KeyName == "left bracket")
	{
		SoundFontIncrement(-1);
	}
	else if (KeyName == "num *")
	{
		MidiIncrement(1);
	}
	else if (KeyName == "num -")
	{
		MidiIncrement(-1);
	}
	else if (KeyName == "delete")
	{
		ToggleAuto();
		return;
	}
	else if (KeyName == "space bar")
	{
		Sustain = !Sustain;
		if (Sustain)
		{
			fluid_synth_all_notes_off(vpsynth, 1);
		}
		return;
	}

	// Get note integer
	int note = LetterToNote(KeyName);
	if (note == -1)
	{
		return;
	}

	// Play note
	fluid_synth_noteon(vpsynth, Channel, note, 127);
	//UE_LOG(LogTemp, Warning, TEXT("%d"), note);
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
	fluid_synth_noteoff(vpsynth, Channel, note);
	UE_LOG(LogTemp, Warning, TEXT("Key Released: %d"), note);
}

// Sets default values
ABase_Piano_Pawn::ABase_Piano_Pawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FFileManagerGeneric::Get().FindFiles(Fonts, *FPaths::ProjectContentDir().Append("SoundFont/"));
	FFileManagerGeneric::Get().FindFiles(Midis, *FPaths::ProjectContentDir().Append("Midi/"));
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

void ABase_Piano_Pawn::Initialize()
{
	fluid_settings_t* settings;

	settings = new_fluid_settings();
	vpsynth = new_fluid_synth(settings);
	midisynth = new_fluid_synth(settings);

	// Create the audio driver. The synthesizer starts playing as soon as the driver is created.
	fluid_audio_driver_t* adriver = new_fluid_audio_driver(settings, vpsynth);
	fluid_audio_driver_t* adriver2 = new_fluid_audio_driver(settings, midisynth);

	// Load a SoundFont and reset presets (so that new instruments get used from the SoundFont)
	for (auto& fontFileName : Fonts)
	{
		const ANSICHAR* FontPath = TCHAR_TO_ANSI(*FPaths::ProjectContentDir().Append("SoundFont/" + fontFileName));
		if (fluid_synth_sfload(vpsynth, FontPath, 1) == FLUID_FAILED || fluid_synth_sfload(midisynth, FontPath, 1) == FLUID_FAILED)
		{
			UE_LOG(LogTemp, Warning, TEXT("Loading the SoundFont '%s' failed!"), *fontFileName);
			return;
		}
	}

	for (int i = 0; i < Fonts.Num(); i++)
	{
		if (Fonts[i] == DefaultFont)
		{
			Channel = i + 1;
		}
	}
	fluid_synth_set_gain(vpsynth, Gain);

	// Print all instruments
	PrintAllInstruments(midisynth, Channel);
}

void ABase_Piano_Pawn::OnEndPlay()
{
	fluid_player_stop(fluid_player);
	for (int i = 0; i < Fonts.Num(); i++)
	{
		fluid_synth_all_notes_off(vpsynth, i + 1);
		fluid_synth_all_sounds_off(vpsynth, i + 1);

		fluid_synth_all_notes_off(midisynth, i + 1);
		fluid_synth_all_sounds_off(midisynth, i + 1);
	}
}

// Called when the game starts or when spawned
void ABase_Piano_Pawn::BeginPlay()
{
	Super::BeginPlay();
	
	Initialize();
}

void ABase_Piano_Pawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnEndPlay();
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
