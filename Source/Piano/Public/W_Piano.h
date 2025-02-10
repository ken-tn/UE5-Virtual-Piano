// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_Piano.generated.h"

/**
 * Widget definition for the PianoPawn class. Create a derived blueprint widget to use.
 */
UCLASS()
class PIANO_API UW_Piano : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	class UButton* ExampleButton;

private:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* FontLabel;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* InstrumentLabel;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TranspositionLabel;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* GainLabel;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* MidiLabel;
};