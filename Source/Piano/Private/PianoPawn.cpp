// Fill out your copyright notice in the Description page of Project Settings.


#include "PianoPawn.h"
#include "fluidsynth.h"
#include "HAL/FileManagerGeneric.h"
#include "W_Piano.h"
#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"

#pragma region Instantiate

void APianoPawn::Initialize()
{
	// Something went horribly wrong, but this prevents a full crash.
	if (this == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("APianoPawn::Initialize failed"));
		return;
	}

	// Add UI to Viewport
	PianoWidget = CreateWidget<UW_Piano>(Cast<APlayerController>(GetController()), WidgetClass);
	if (PianoWidget != nullptr)
	{
		// Construct keyboard layout widget
		UUserWidget* KeysWidget = PianoWidget->WidgetTree->ConstructWidget<UUserWidget>(PianoWidget->WBP_PianoKeyLayout, TEXT("KeysLayout"));
		UGridPanel* KeysPanel = PianoWidget->WidgetTree->FindWidget<UGridPanel>(FName("KeysPanel"));
		if (KeysPanel)
		{
			KeysPanel->AddChildToGrid(KeysWidget, 0, 0);
		}
		
		PianoWidget->AddToViewport();
	}

	// Clamp DefaultFont to font limit
	DefaultFont = FMath::Clamp(DefaultFont, 0, Fonts.Num() - 1);

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
	MidiIncrement(0);
	GainIncrement(0);
}

// Sets default values
APianoPawn::APianoPawn()
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

#pragma endregion Instantiate

#pragma region Functionality
void APianoPawn::ToggleAuto()
{
	// [Ready, Playing, Stopping (unused), Done]
	int fluidStatus = fluid_player_get_status(fluid_player);
	if (fluidStatus == FLUID_PLAYER_PLAYING)
	{
		fluid_player_stop(fluid_player);
	}
	else if (fluidStatus == FLUID_PLAYER_READY)
	{
		fluid_player_play(fluid_player);
	}
}

void APianoPawn::TransposeIncrement(int Increment)
{
	Transposition += Increment;
	TransposeChanged.Broadcast(Transposition);
}

void APianoPawn::GainIncrement(float Increment)
{
	Gain += Increment;
	fluid_synth_set_gain(vpsynth, Gain);
	fluid_synth_set_gain(midisynth, Gain);
	GainChanged.Broadcast(Gain);
}

int APianoPawn::ChangeInstrument(int fontid, int programindex)
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

void APianoPawn::InstrumentIncrement(int Increment)
{
	CurrentProgram[FontIndex] += Increment;
	if (ChangeInstrument(FontID, CurrentProgram[FontIndex]) == FLUID_FAILED)
	{
		CurrentProgram[FontIndex] -= Increment;
	}
}

int APianoPawn::LoadSoundfont(int fontIndex)
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
		UE_LOG(LogTemp, Error, TEXT("Loading the SoundFont '%hs' failed!"), FontPath);
		return FLUID_FAILED;
	}

	LoadedFonts.Add(font, fontFileName);
	FontID = font;
	return FLUID_OK;
}

void APianoPawn::SoundFontIncrement(int Increment)
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

void APianoPawn::MidiIncrement(int Increment)
{
	MidiIndex += Increment;
	if (MidiIndex > Midis.Num() || MidiIndex < 0)
	{
		MidiIndex -= Increment;
		return;
	}

	int wasAutoPlaying = fluid_player_get_status(fluid_player) == FLUID_PLAYER_PLAYING;
	delete_fluid_player(fluid_player);

	// Create auto player
	const FString midiFileName = Midis[MidiIndex];
	fluid_player = new_fluid_player(midisynth);
	fluid_player_add(fluid_player, TCHAR_TO_ANSI(*FPaths::ProjectContentDir().Append("Midi/" + midiFileName)));

	// Automatically play if autoplay was already on
	if (wasAutoPlaying)
	{
		fluid_player_play(fluid_player);
	}
	MidiChanged.Broadcast(midiFileName, MidiIndex);
	UE_LOG(LogTemp, Display, TEXT("Midi track: %s"), *midiFileName);
}

int APianoPawn::LetterToNote(const FString KeyName)
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

void APianoPawn::TriggerTransposeUp()
{
	TransposeIncrement(1);
}

void APianoPawn::TriggerTransposeDown()
{
	TransposeIncrement(-1);
}

void APianoPawn::TriggerGainIncrementUp()
{
	GainIncrement(0.01f);
}

void APianoPawn::TriggerGainIncrementDown()
{
	GainIncrement(-0.01f);
}

void APianoPawn::TriggerInstrumentNext()
{
	InstrumentIncrement(1);
}

void APianoPawn::TriggerInstrumentPrevious()
{
	InstrumentIncrement(-1);
}

void APianoPawn::TriggerSoundfontNext()
{
	SoundFontIncrement(1);
}

void APianoPawn::TriggerSoundfontPrevious()
{
	SoundFontIncrement(-1);
}

void APianoPawn::TriggerMIDINext()
{
	MidiIncrement(1);
}

void APianoPawn::TriggerMIDIPrevious()
{
	MidiIncrement(-1);
}

void APianoPawn::TriggerAutoplayToggle()
{
	ToggleAuto();
}

void APianoPawn::TriggerSustainPressed()
{
	Sustain = !Sustain;
	if (Sustain)
	{
		fluid_synth_all_notes_off(vpsynth, 1);
	}
	return;
}

void APianoPawn::TriggerSustainReleased()
{
	Sustain = !Sustain;
	return;
}

void APianoPawn::OnKeyDown(FKey Key)
{
	const FString KeyName = *Key.GetDisplayName().ToString().ToLower();
	//UE_LOG(LogTemp, Display, TEXT("Key: %s"), *KeyName);

	// Get note integer
	int note = LetterToNote(KeyName);
	if (note == -1)
	{
		return;
	}

	// Play note
	// Client-specific functionality
	SetCurrentNote(note);
}

void APianoPawn::SetCurrentNote(float note)
{
	//UE_LOG(LogTemp, Display, TEXT("role: %s"), (GetLocalRole() == ROLE_Authority) ? *FString("true") : *FString("false"));
	/*if (GetLocalRole() == ROLE_Authority)
	{
	}*/
	CurrentNote = note;
	OnNotePlayed();
}

void APianoPawn::OnKeyUp(FKey Key)
{
	const FString KeyName = *Key.GetDisplayName().ToString().ToLower();

	int note = LetterToNote(KeyName);
	if (note == -1 || !Sustain)
	{
		return;
	}

	// Release note
	fluid_synth_noteoff(vpsynth, 0, note);
}

// Called to bind functionality to input
void APianoPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("PianoKeyboardDown", IE_Pressed, this, &APianoPawn::OnKeyDown);
	PlayerInputComponent->BindAction("PianoKeyboardUp", IE_Released, this, &APianoPawn::OnKeyUp);

	// Setup enhanced input
	if (ULocalPlayer* LocalPlayer = Cast<APlayerController>(GetController())->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// Set the mapping context
			if (!InputMapping.IsNull())
			{
				InputSystem->ClearAllMappings();
				InputSystem->AddMappingContext(InputMapping.LoadSynchronous(), 0);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("SetupPlayerInputComponent: Failed to bind context."));
			}

			// Bind the input actions to a method
			if (InputConfigData != nullptr)
			{
				UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
				UInputActionsConfig* Config = NewObject<UInputActionsConfig>(this, InputConfigData);

				if (Config->IA_TransposeUp)
				{
					Input->BindAction(Config->IA_TransposeUp, ETriggerEvent::Triggered, this, &APianoPawn::TriggerTransposeUp);
				}

				if (Config->IA_TransposeDown)
				{
					Input->BindAction(Config->IA_TransposeDown, ETriggerEvent::Triggered, this, &APianoPawn::TriggerTransposeDown);
				}

				if (Config->IA_GainIncrementUp)
				{
					Input->BindAction(Config->IA_GainIncrementUp, ETriggerEvent::Triggered, this, &APianoPawn::TriggerGainIncrementUp);
				}

				if (Config->IA_GainIncrementDown)
				{
					Input->BindAction(Config->IA_GainIncrementDown, ETriggerEvent::Triggered, this, &APianoPawn::TriggerGainIncrementDown);
				}

				if (Config->IA_InstrumentNext)
				{
					Input->BindAction(Config->IA_InstrumentNext, ETriggerEvent::Triggered, this, &APianoPawn::TriggerInstrumentNext);
				}

				if (Config->IA_InstrumentPrevious)
				{
					Input->BindAction(Config->IA_InstrumentPrevious, ETriggerEvent::Triggered, this, &APianoPawn::TriggerInstrumentPrevious);
				}

				if (Config->IA_SoundfontNext)
				{
					Input->BindAction(Config->IA_SoundfontNext, ETriggerEvent::Triggered, this, &APianoPawn::TriggerSoundfontNext);
				}

				if (Config->IA_SoundfontPrevious)
				{
					Input->BindAction(Config->IA_SoundfontPrevious, ETriggerEvent::Triggered, this, &APianoPawn::TriggerSoundfontPrevious);
				}

				if (Config->IA_MIDINext)
				{
					Input->BindAction(Config->IA_MIDINext, ETriggerEvent::Triggered, this, &APianoPawn::TriggerMIDINext);
				}

				if (Config->IA_MIDIPrevious)
				{
					Input->BindAction(Config->IA_MIDIPrevious, ETriggerEvent::Triggered, this, &APianoPawn::TriggerMIDIPrevious);
				}

				if (Config->IA_AutoplayToggle)
				{
					Input->BindAction(Config->IA_AutoplayToggle, ETriggerEvent::Triggered, this, &APianoPawn::TriggerAutoplayToggle);
				}

				if (Config->IA_SustainPressed)
				{
					Input->BindAction(Config->IA_SustainPressed, ETriggerEvent::Triggered, this, &APianoPawn::TriggerSustainPressed);
				}

				if (Config->IA_SustainReleased)
				{
					Input->BindAction(Config->IA_SustainReleased, ETriggerEvent::Triggered, this, &APianoPawn::TriggerSustainReleased);
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("SetupPlayerInputComponent: Failed to bind actions."));
			}
		}
	}
}

#pragma endregion Inputs

#pragma region Debugging
void APianoPawn::PrintAllInstruments(fluid_synth_t* synth, int sfont_id)
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
void APianoPawn::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	DefaultFont = FMath::Clamp(DefaultFont, 0, Fonts.Num()-1);
}
#endif

#pragma endregion Debugging

#pragma region Networking

void APianoPawn::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current note.
	DOREPLIFETIME(APianoPawn, CurrentNote);
}

void APianoPawn::OnNotePlayed()
{
	if (IsLocallyControlled())
	{
		//UE_LOG(LogTemp, Display, TEXT("PLAYING NOTE %d"), CurrentNote);
		fluid_synth_noteon(vpsynth, 0, CurrentNote, 127);
	}
	CurrentNote = -1;
}

void APianoPawn::OnRep_CurrentNote()
{
	OnNotePlayed();
}

#pragma endregion Networking

// Called when the game starts or when spawned
void APianoPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called when game ends or destroyed
void APianoPawn::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	fluid_player_stop(fluid_player);
	fluid_synth_all_notes_off(vpsynth, 0);
	fluid_synth_all_notes_off(midisynth, 0);
	fluid_synth_all_sounds_off(vpsynth, 0);
	fluid_synth_all_sounds_off(midisynth, 0);
	UE_LOG(LogTemp, Display, TEXT("%s"), (mididriver != NULL) ? *FString("true") : *FString("false"));
	//delete_fluid_audio_driver(vpdriver);
	//delete_fluid_audio_driver(mididriver);
}

// Called every frame
void APianoPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

