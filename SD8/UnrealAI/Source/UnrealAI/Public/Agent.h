// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Agent.generated.h"

UCLASS()
class UNREALAI_API AAgent : public ACharacter
{
	GENERATED_BODY()

	/* The collider for the area of vision of the Agent, where it can detect other Agents */
	UPROPERTY( Category = AI, VisibleDefaultsOnly, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
		class USphereComponent* VisionSphere;
	
public:	
	// Sets default values for this actor's properties
	AAgent();

	/* The weight of the Alignment vector component */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AI )
		float AlignmentWeight;

	/* The weight of the Cohesion vector component */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AI )
		float CohesionWeight;

	/* The weight of the Separation vector component */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AI )
		float SeparationWeight;

	/* The base movement speed for the Agents */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AI )
		float BaseMovementSpeed;

	/* The maximum movement speed the Agents can have */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AI )
		float MaxMovementSpeed;

	/* The maximum radius at which the Agent can detect other Agents */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AI )
		float VisionRadius;

	/* This event is called every tick, before applying the calculated move vector to move the Agent */
	UFUNCTION( BlueprintImplementableEvent, Category = AI )
		void CalculateNewMoveVector();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

protected:
	/* The movement vector (in local) this agent should move this tick. */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AI )
		FVector NewMoveVector;

	/* The movement vector (in local) this agent had last tick. */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = AI )
		FVector CurrentMoveVector;
};
