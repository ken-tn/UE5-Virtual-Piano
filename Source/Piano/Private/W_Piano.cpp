// Fill out your copyright notice in the Description page of Project Settings.


#include "W_Piano.h"

UW_Piano::UW_Piano(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UW_Piano::NativeConstruct()
{
	// Do some custom setup

	// Call the Blueprint "Event Construct" node
	Super::NativeConstruct();
}