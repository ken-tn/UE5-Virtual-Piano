// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_Piano_Pawn.h"

void ABase_Piano_Pawn::OnKeyDown(FKey key) {
	UE_LOG(LogTemp, Warning, TEXT("key: %s"), *key.GetDisplayName().ToString());
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
