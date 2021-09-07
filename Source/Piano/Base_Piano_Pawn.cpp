// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_Piano_Pawn.h"
#include <fluidsynth.h>
#include "HAL/FileManagerGeneric.h"
#include "W_Piano.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

#pragma region Instantiate

void ABase_Piano_Pawn::Initialize()
{
	PianoWidget = CreateWidget<UW_Piano>(Cast<APlayerController>(GetController()), WidgetClass);
	if (PianoWidget != nullptr)
	{
		PianoWidget->AddToViewport();
	}

	fluid_settings_t* settings = new_fluid_settings();
	//fluid_settings_setint(settings, "synth.verbose", true);

	vpsynth = new_fluid_synth(settings);
	midisynth = new_fluid_synth(settings);
	fluid_synth_set_gain(vpsynth, Gain);

	// Create the audio driver. The synthesizer starts playing as soon as the driver is created.
	vpdriver = new_fluid_audio_driver(settings, vpsynth);
	mididriver = new_fluid_audio_driver(settings, midisynth);

	if (vpdriver == NULL || mididriver == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create driver"));
	}

	LoadSoundfont(DefaultFont);
	FontIndex = DefaultFont;

	// Create fluid player
	fluid_player = new_fluid_player(midisynth);
	fluid_player_add(fluid_player, TCHAR_TO_ANSI(*FPaths::ProjectContentDir().Append("Midi/" + Midis[0])));

	// Print all instruments
	//PrintAllInstruments(midisynth, Channel);

	// Broadcast events
	SoundFontIncrement(0);
}

// Sets default values
ABase_Piano_Pawn::ABase_Piano_Pawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FFileManagerGeneric::Get().FindFiles(Fonts, *FPaths::ProjectContentDir().Append("SoundFont/"));
	FFileManagerGeneric::Get().FindFiles(Midis, *FPaths::ProjectContentDir().Append("Midi/"));
	ConstructorHelpers::FClassFinder<UUserWidget>WBP(TEXT("/Game/UI/WBP_Piano"));

	if (WBP.Class != nullptr)
	{
		WidgetClass = WBP.Class;
	}

	CurrentNote = 1;
}

void ABase_Piano_Pawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	fluid_player_stop(fluid_player);
	fluid_synth_all_notes_off(vpsynth, 0);
	fluid_synth_all_notes_off(midisynth, 0);
	fluid_synth_all_sounds_off(vpsynth, 0);
	fluid_synth_all_sounds_off(midisynth, 0);
	UE_LOG(LogTemp, Display, TEXT("%s"), (mididriver != NULL) ? *FString("true") : *FString("false"));
	//delete_fluid_audio_driver(vpdriver);
	//delete_fluid_audio_driver(mididriver);
}

#pragma endregion Instantiate

#pragma region Functionality
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
}

void ABase_Piano_Pawn::GainIncrement(float Increment)
{
	Gain += Increment;
	fluid_synth_set_gain(vpsynth, Gain);
	fluid_synth_set_gain(midisynth, Gain);
	GainChanged.Broadcast(Gain);
}

int ABase_Piano_Pawn::ChangeInstrument(int fontid, int programindex)
{
	if (programindex < 0)
	{
		return FLUID_FAILED;
	}
	fluid_sfont_t* sfont = fluid_synth_get_sfont_by_id(vpsynth, fontid);
	fluid_preset_t* preset;

	// iterate to font
	fluid_sfont_iteration_start(sfont);
	for (int i = 0; i < programindex; i++)
	{
		preset = fluid_sfont_iteration_next(sfont);
	}

	if ((preset = fluid_sfont_iteration_next(sfont)) != NULL)
	{
		fluid_synth_program_select(vpsynth, 0, FontID, fluid_synth_get_bank_offset(vpsynth, FontID), programindex);
		fluid_synth_program_select(midisynth, 0, FontID, fluid_synth_get_bank_offset(midisynth, FontID), programindex);
		InstrumentChanged.Broadcast(FString(fluid_preset_get_name(preset)), programindex);
		return FLUID_OK;
	}
	else
	{
		return FLUID_FAILED;
	}
}

void ABase_Piano_Pawn::InstrumentIncrement(int Increment)
{
	CurrentProgram[FontIndex] += Increment;
	if (ChangeInstrument(FontID, CurrentProgram[FontIndex]) == FLUID_FAILED)
	{
		CurrentProgram[FontIndex] -= Increment;
	}
}

int ABase_Piano_Pawn::LoadSoundfont(int fontIndex)
{
	FString fontFileName = Fonts[fontIndex];
	for (auto& pair : LoadedFonts)
	{
		if (pair.Value == fontFileName)
		{
			FontID = pair.Key;
			fluid_synth_program_select(vpsynth, 0, FontID, fluid_synth_get_bank_offset(vpsynth, FontID), CurrentProgram[fontIndex]);
			fluid_synth_program_select(midisynth, 0, FontID, fluid_synth_get_bank_offset(midisynth, FontID), CurrentProgram[fontIndex]);
			return FLUID_OK;
		}
	}

	// Load a SoundFont and reset presets (so that new instruments get used from the SoundFont)
	const ANSICHAR* FontPath = TCHAR_TO_ANSI(*FPaths::ProjectContentDir().Append("SoundFont/" + fontFileName));
	int font = fluid_synth_sfload(vpsynth, FontPath, 1);
	int font2 = fluid_synth_sfload(midisynth, FontPath, 1);
	fluid_synth_set_bank_offset(vpsynth, font, font);
	fluid_synth_set_bank_offset(midisynth, font2, font2);
	//UE_LOG(LogTemp, Display, TEXT("LOADED %s AS FONTID %d"), *fontFileName, font);
	if (font == FLUID_FAILED || font2 == FLUID_FAILED)
	{
		UE_LOG(LogTemp, Error, TEXT("Loading the SoundFont '%s' failed!"), FontPath);
		return FLUID_FAILED;
	}

	LoadedFonts.Add(font, fontFileName);
	FontID = font;
	return FLUID_OK;
}

void ABase_Piano_Pawn::SoundFontIncrement(int Increment)
{
	FontIndex += Increment;
	if (FontIndex > Fonts.Num()-1 || FontIndex < 0)
	{
		FontIndex -= Increment;
		return;
	}

	// load new fonts
	LoadSoundfont(FontIndex);
	InstrumentIncrement(0);
	FontChanged.Broadcast(Fonts[FontIndex], FontIndex);
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
	UE_LOG(LogTemp, Display, TEXT("Midi track: %s"), *midiFileName);
}

int ABase_Piano_Pawn::LetterToNote(const FString KeyName)
{
	int note = letterNoteMap.Find(KeyName);

	if (note == -1)
	{
		return note;
	}
	else
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
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
			return note + offset + 36; // +36 :thonk:
		}
		else
		{
			return -1;
		}
	}
}
#pragma endregion Functionality

#pragma region Inputs

void ABase_Piano_Pawn::OnKeyDown(FKey Key)
{
	const FString KeyName = *Key.GetDisplayName().ToString().ToLower();
	//UE_LOG(LogTemp, Display, TEXT("Key: %s"), *KeyName);
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
	else if (KeyName == "end")
	{
		InstrumentIncrement(1);
		return;
	}
	else if (KeyName == "home")
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
	//Client-specific functionality
	SetCurrentNote(note);
}

void ABase_Piano_Pawn::SetCurrentNote(float note)
{
	//UE_LOG(LogTemp, Display, TEXT("role: %s"), (GetLocalRole() == ROLE_Authority) ? *FString("true") : *FString("false"));
	/*if (GetLocalRole() == ROLE_Authority)
	{
	}*/
	CurrentNote = note;
	OnNotePlayed();
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
	fluid_synth_noteoff(vpsynth, 0, note);
}

// Called to bind functionality to input
void ABase_Piano_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("PianoKeyboardDown", IE_Pressed, this, &ABase_Piano_Pawn::OnKeyDown);
	PlayerInputComponent->BindAction("PianoKeyboardUp", IE_Released, this, &ABase_Piano_Pawn::OnKeyUp);
}

#pragma endregion Inputs

#pragma region Debugging
void ABase_Piano_Pawn::PrintAllInstruments(fluid_synth_t* synth, int sfont_id)
{
	fluid_preset_t* preset;

	fluid_sfont_t* sfont = fluid_synth_get_sfont_by_id(synth, sfont_id);
	int offset = fluid_synth_get_bank_offset(synth, sfont_id);

	fluid_sfont_iteration_start(sfont);
	while ((preset = fluid_sfont_iteration_next(sfont)) != NULL)
	{
		UE_LOG(LogTemp, Display, TEXT("%03d-%03d %s\n"),
			fluid_preset_get_banknum(preset) + offset,
			fluid_preset_get_num(preset),
			*FString(fluid_preset_get_name(preset)));
	}
}

#if WITH_EDITOR
void ABase_Piano_Pawn::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	DefaultFont = FMath::Clamp(DefaultFont, 0, Fonts.Num()-1);
}
#endif

#pragma endregion Debugging

#pragma region Networking

void ABase_Piano_Pawn::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current note.
	DOREPLIFETIME(ABase_Piano_Pawn, CurrentNote);
}

void ABase_Piano_Pawn::OnNotePlayed()
{
	if (IsLocallyControlled())
	{
		//UE_LOG(LogTemp, Display, TEXT("PLAYING NOTE %d"), CurrentNote);
		fluid_synth_noteon(vpsynth, 0, CurrentNote, 127);
	}
	CurrentNote = -1;
}

void ABase_Piano_Pawn::OnRep_CurrentNote()
{
	OnNotePlayed();
}

#pragma endregion Networking

// Called when the game starts or when spawned
void ABase_Piano_Pawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABase_Piano_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

