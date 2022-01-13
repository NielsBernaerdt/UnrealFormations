// Copyright Epic Games, Inc. All Rights Reserved.
#include "UnrealFormationsGameMode.h"
#include "UnrealFormationsPlayerController.h"
#include "UnrealFormationsCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUnrealFormationsGameMode::AUnrealFormationsGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AUnrealFormationsPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}