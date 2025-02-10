// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_PianoKey.generated.h"

/**
 * 
 */
UCLASS()
class PIANO_API UW_PianoKey : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Button;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* KeyTextBlock;
	
};
