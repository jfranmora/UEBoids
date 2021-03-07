// Fill out your copyright notice in the Description page of Project Settings.

#include "Boid.h"
#include "UEFlockingGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#define DEBUG_MESSAGE(x, ...) if(GEngine){ GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT(x), __VA_ARGS__)); }

// Sets default values
ABoid::ABoid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();

	InitBoid();

	// Register in game mode
	AUEFlockingGameModeBase* CurrentGameMode = Cast<AUEFlockingGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));	
	CurrentGameMode->BoidsArray.Add(this);
}

void ABoid::InitBoid()
{
	// Set random rotation
	FRotator Rotator = UKismetMathLibrary::RandomRotator();
	this->SetActorRotation(Rotator, ETeleportType::None);
	
	ForwardVector = this->GetActorForwardVector();
}

void ABoid::MoveBoid(float DeltaTime)
{
	FVector CurrentPos = this->GetActorLocation();
	FVector ForwardDir = this->GetActorForwardVector();
	FVector NewPos = CurrentPos + ForwardDir * MoveSpeed * DeltaTime;
	this->SetActorLocation(NewPos);
}

void ABoid::Steer()
{
	// If close to limits, return to center
	FVector CurrentPos = this->GetActorLocation();
	float CurrentPosX = FMath::Abs(CurrentPos.X);
	float CurrentPosY = FMath::Abs(CurrentPos.Y);
	float CurrentPosZ = FMath::Abs(CurrentPos.Z);
	if (CurrentPosX > MaxCenterDistance || CurrentPosY > MaxCenterDistance || CurrentPosZ > MaxCenterDistance)
	{
		ForwardVector = -CurrentPos;
		return;
	}

	FVector MyLocation = this->GetActorLocation();
	
	// Get current game mode
	AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(GetWorld());
	AUEFlockingGameModeBase* CurrentGameMode = Cast<AUEFlockingGameModeBase>(BaseGameMode);

	// Iterate over all boids
	TArray<ABoid*> BoidsArray = CurrentGameMode->BoidsArray;

	FVector NeighboursLocationSum = FVector::ZeroVector;
	FVector NeighboursForwardSum = FVector::ZeroVector;
	FVector NeighboursSeparationDir = FVector::ZeroVector;

	int Neighbours = 0;
	int NeighboursToSeparate = 0;

	// Iterate over all boids to detect neighbours
	for (int i = 0; i < BoidsArray.Num(); ++i)
	{
		ABoid* OtherBoid = BoidsArray[i];
		if (OtherBoid == this || OtherBoid == nullptr)
			continue;

		FVector OtherBoidLocation = OtherBoid->GetActorLocation();
		if (UKismetMathLibrary::Vector_Distance(MyLocation, OtherBoidLocation) > DetectDistance)
			continue;

		Neighbours++;
		
		// Cohesion
		NeighboursLocationSum += OtherBoidLocation;

		// Alignment
		NeighboursForwardSum += OtherBoid->GetActorForwardVector();
		
		// Separation
		if (UKismetMathLibrary::Vector_Distance(MyLocation, OtherBoidLocation) < SeparationDistance)
		{
			NeighboursToSeparate++;
			
			// 1 - (Normalized separation direction)
			NeighboursSeparationDir += (MyLocation - OtherBoidLocation) / SeparationDistance;		
		}
	}

	// Perform calculations
	if (Neighbours > 0)
	{
		// Calculate target dir
		FVector NewForwardVector = FVector::ZeroVector;
		if (CurrentGameMode->GetCohesionEnabled())
		{
			NeighboursLocationSum /= Neighbours;
			FVector CohesionDir = NeighboursLocationSum - MyLocation;
			CohesionDir.Normalize();
			
			NewForwardVector += CohesionDir;
		}

		if (CurrentGameMode->GetAlignmentEnabled())
		{
			FVector AlignmentDir = NeighboursForwardSum /= Neighbours;
			NewForwardVector += AlignmentDir;
		}

		if (CurrentGameMode->GetSeparationEnabled())
		{
			FVector SeparationDir = NeighboursSeparationDir;
			FVector RandomDir = UKismetMathLibrary::RandomUnitVector();
			NewForwardVector += 2.f * SeparationDir + RandomDir * .35f;
		}

		// Change forward direction
		if (NewForwardVector != FVector::ZeroVector)
		{
			NewForwardVector.Normalize();		
			ForwardVector = NewForwardVector;	
		}
	}
}

void ABoid::RotateBoid(float DeltaTime)
{
	FRotator NewRotator = UKismetMathLibrary::RLerp(this->GetActorRotation(), ForwardVector.Rotation(), 5.f * DeltaTime, true);
	
	// Rotate to look to new vector
	this->SetActorRotation(NewRotator);
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Steer();
	RotateBoid(DeltaTime);
	MoveBoid(DeltaTime);
}
