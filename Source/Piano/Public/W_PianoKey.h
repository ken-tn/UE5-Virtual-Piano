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

public:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* Anim_OnPressed;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* Anim_OnReleased;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Button;
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* KeyTextBlock;
	
};
