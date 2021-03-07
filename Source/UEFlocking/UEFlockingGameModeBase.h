// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Boid.h"
#include "GameFramework/GameModeBase.h"
#include "UEFlockingGameModeBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UEFLOCKING_API AUEFlockingGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	public:
		AUEFlockingGameModeBase();
		virtual void BeginPlay() override;
	
		UPROPERTY(EditDefaultsOnly)
		UClass* BoidBlueprint; 

		UFUNCTION(BlueprintCallable, Category="Boids")
        void SpawnBoids();
	
		void ToggleCohesion();
		void ToggleAlignment();
		void ToggleSeparation();
		
		void SetCohesionEnabled(bool enabled);
		void SetAlignmentEnabled(bool enabled);
		void SetSeparationEnabled(bool enabled);
		bool GetCohesionEnabled();
		bool GetAlignmentEnabled();
		bool GetSeparationEnabled();

		TArray<ABoid*> BoidsArray;

	private:
		bool CohesionEnabled;
		bool AlignmentEnabled;
		bool SeparationEnabled;
};
