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
	this->SetActorRotation(UKismetMathLibrary::RandomRotator(), ETeleportType::None);
	ForwardVector = this->GetActorForwardVector();
}

void ABoid::MoveBoid(float DeltaTime)
{
	const FVector CurrentPos = this->GetActorLocation();
	const FVector ForwardDir = this->GetActorForwardVector();
	this->SetActorLocation(CurrentPos + ForwardDir * MoveSpeed * DeltaTime);
}

void ABoid::Steer()
{
	// If close to limits, return to center
	const FVector CurrentPos = this->GetActorLocation();
	const float DistanceToBorder = GetDistanceToBorder();
	if (DistanceToBorder > MaxCenterDistance)
	{
		ForwardVector = -CurrentPos;
		return;
	}

	const FVector MyLocation = this->GetActorLocation();
	
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
		const ABoid* OtherBoid = BoidsArray[i];
		if (OtherBoid == this || OtherBoid == nullptr)
			continue;

		FVector OtherBoidLocation = OtherBoid->GetActorLocation();
		const float BoidsDistance = UKismetMathLibrary::Vector_Distance(MyLocation, OtherBoidLocation);
		if (BoidsDistance > DetectDistance)
			continue;

		Neighbours++;
		
		// Cohesion
		NeighboursLocationSum += OtherBoidLocation;

		// Alignment
		NeighboursForwardSum += OtherBoid->GetActorForwardVector();
		
		// Separation
		if (BoidsDistance < SeparationDistance)
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
			const FVector SeparationDir = NeighboursSeparationDir;
			const FVector RandomDir = UKismetMathLibrary::RandomUnitVector();
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
	const FRotator NewRotator = UKismetMathLibrary::RLerp(this->GetActorRotation(), ForwardVector.Rotation(), DeltaTime, true);
	this->SetActorRotation(NewRotator);
}

float ABoid::GetDistanceToBorder() const
{
	const FVector CurrentPos = this->GetActorLocation();
	return FMath::Max3(FMath::Abs(CurrentPos.X), FMath::Abs(CurrentPos.Y), FMath::Abs(CurrentPos.Z));
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Steer();
	RotateBoid(DeltaTime);
	MoveBoid(DeltaTime);
}
