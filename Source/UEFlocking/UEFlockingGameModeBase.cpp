// Copyright Epic Games, Inc. All Rights Reserved.

#include "UEFlockingGameModeBase.h"

#include "Kismet/GameplayStatics.h"

#define DEBUG_MESSAGE(x, ...) if(GEngine){ GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT(x), __VA_ARGS__)); }

AUEFlockingGameModeBase::AUEFlockingGameModeBase()
{
	SetCohesionEnabled(true);
	SetAlignmentEnabled(true);
	SetSeparationEnabled(true);
}

void AUEFlockingGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	const APlayerController* ControlledCharacter = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (ControlledCharacter != nullptr)
	{		
		UInputComponent* ControllerInputComponent = ControlledCharacter->InputComponent;
		if (ControllerInputComponent)
		{
			DEBUG_MESSAGE("Registered inputs");
			
			ControllerInputComponent->BindAction("SpawnBoids", IE_Pressed, this, &AUEFlockingGameModeBase::SpawnBoids);
			ControllerInputComponent->BindAction("ToggleCohesion", IE_Pressed, this, &AUEFlockingGameModeBase::ToggleCohesion);
			ControllerInputComponent->BindAction("ToggleAlignment", IE_Pressed, this, &AUEFlockingGameModeBase::ToggleAlignment);
			ControllerInputComponent->BindAction("ToggleSeparation", IE_Pressed, this, &AUEFlockingGameModeBase::ToggleSeparation);
		}
	}
	
	SpawnBoids();
}

void AUEFlockingGameModeBase::SpawnBoids()
{
	for (int i = 0; i < 512; i++)
	{
		FActorSpawnParameters SpawnInfo;
		GetWorld()->SpawnActor<ABoid>(BoidBlueprint, FVector::ZeroVector, FRotator::ZeroRotator, SpawnInfo);
	}
}

void AUEFlockingGameModeBase::ToggleCohesion()
{
	SetCohesionEnabled(!CohesionEnabled);
}

void AUEFlockingGameModeBase::ToggleAlignment()
{
	SetAlignmentEnabled(!AlignmentEnabled);
}

void AUEFlockingGameModeBase::ToggleSeparation()
{
	SetSeparationEnabled(!SeparationEnabled);
}

void AUEFlockingGameModeBase::SetCohesionEnabled(bool enabled)
{
	CohesionEnabled = enabled;
	DEBUG_MESSAGE("CohesionEnabled: %d", CohesionEnabled);
}

void AUEFlockingGameModeBase::SetAlignmentEnabled(bool enabled)
{
	AlignmentEnabled = enabled;
	DEBUG_MESSAGE("AlignmentEnabled: %d", AlignmentEnabled);
}

void AUEFlockingGameModeBase::SetSeparationEnabled(bool enabled)
{
	SeparationEnabled = enabled;
	DEBUG_MESSAGE("SeparationEnabled: %d", SeparationEnabled);
}

bool AUEFlockingGameModeBase::GetCohesionEnabled()
{
	return CohesionEnabled;
}

bool AUEFlockingGameModeBase::GetAlignmentEnabled()
{
	return AlignmentEnabled;
}

bool AUEFlockingGameModeBase::GetSeparationEnabled()
{
	return SeparationEnabled;
}
