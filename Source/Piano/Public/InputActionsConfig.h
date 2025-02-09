// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "InputActionsConfig.generated.h"

/**
 * Input configuration. Create an instance of this in the editor and set all InputAction bindings.
 */
UCLASS(Blueprintable)
class PIANO_API UInputActionsConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_TransposeUp;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_TransposeDown;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_GainIncrementUp;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_GainIncrementDown;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_InstrumentNext;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_InstrumentPrevious;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_SoundfontNext;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_SoundfontPrevious;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MIDINext;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MIDIPrevious;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_AutoplayToggle;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_SustainPressed;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_SustainReleased;

	UInputActionsConfig();
	~UInputActionsConfig();
};
