// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealAI.h"
#include "Agent.h"


// Sets default values
AAgent::AAgent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initializing default values
	BaseMovementSpeed = 200.0f;
	MaxMovementSpeed = 300.0f;
	AlignmentWeight = 1.0f;
	CohesionWeight = 0.5f;
	SeparationWeight = 4.0f;
	VisionRadius = 400.0f;

	// Create vision sphere
	VisionSphere = CreateDefaultSubobject<USphereComponent>( TEXT( "VisionSphere" ) );
	VisionSphere->AttachTo( RootComponent );
	VisionSphere->SetSphereRadius( VisionRadius );
}

// Called when the game starts or when spawned
void AAgent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize move vector
	NewMoveVector = GetActorRotation().Vector().GetSafeNormal();
}

// Called every frame
void AAgent::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	CurrentMoveVector = NewMoveVector;

	CalculateNewMoveVector();

	const FVector NewDirection = ( NewMoveVector * BaseMovementSpeed * DeltaTime ).GetClampedToMaxSize( MaxMovementSpeed * DeltaTime );
	const FRotator NewRotation = NewMoveVector.Rotation();

	FHitResult Hit( 1.f );
	RootComponent->MoveComponent( NewDirection, NewRotation, true, &Hit );

	if ( Hit.IsValidBlockingHit() )
	{
		const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
		const FVector Deflection = FVector::VectorPlaneProject( NewDirection, Normal2D ) * ( 1.f - Hit.Time );
		RootComponent->MoveComponent( Deflection, NewRotation, true );
	}
}

