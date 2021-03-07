// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boid.generated.h"

UCLASS()
class UEFLOCKING_API ABoid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoid();

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector ForwardVector;
	
	void InitBoid();
	void MoveBoid(float DeltaTime);
	void Steer();
	void RotateBoid(float DeltaTime);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	float MoveSpeed = 300.f;

	UPROPERTY(EditDefaultsOnly)
	float DetectDistance = 300.0f;

	UPROPERTY(EditDefaultsOnly)
	float SeparationDistance = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	float MaxCenterDistance = 10 * 100.0f;
};
