// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealFormationsPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "UnrealFormationsCharacter.h"
#include "Engine/World.h"

AUnrealFormationsPlayerController::AUnrealFormationsPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AUnrealFormationsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	m_vTarget = GetPawn()->GetActorLocation();
}

void AUnrealFormationsPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		SetMouseAstarget();
	}
	SetNewMoveDestination(m_vTarget);
}

void AUnrealFormationsPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AUnrealFormationsPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AUnrealFormationsPlayerController::OnSetDestinationReleased);
}

void AUnrealFormationsPlayerController::SetMouseAstarget()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		m_vTarget = Hit.ImpactPoint;
	}
}

void AUnrealFormationsPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	//APawn* const MyPawn = GetPawn();
	AUnrealFormationsCharacter* MyPawn = Cast<AUnrealFormationsCharacter>(GetPawn());
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		if (Distance > 100)
			MyPawn->AddMovementInput(DestLocation - MyPawn->GetActorLocation());
		else
		{
			MyPawn->AddMovementInput({ 0,0,0 });
			MyPawn->SetCurrentOrientation(MyPawn->GetActorForwardVector());
		}
	}
}

void AUnrealFormationsPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void AUnrealFormationsPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
	AUnrealFormationsCharacter* MyPawn = Cast<AUnrealFormationsCharacter>(GetPawn());
	if (MyPawn)
	{
		MyPawn->SetMoveToCursor(true);
	}
}