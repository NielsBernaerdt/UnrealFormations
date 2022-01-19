// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UnrealFormationsPlayerController.generated.h"

UCLASS()
class AUnrealFormationsPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AUnrealFormationsPlayerController();

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	virtual void BeginPlay() override;

	/** Navigate player to the current mouse cursor location. */
	void SetMouseAstarget();
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

private:
	FVector m_vTarget;
};