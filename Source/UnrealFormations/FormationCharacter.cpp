#include "FormationCharacter.h"
//
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "UnrealFormationsCharacter.h"

// Sets default values
AFormationCharacter::AFormationCharacter()
{
	//Activate ticking
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
}

// Called when the game starts or when spawned
void AFormationCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	SetTarget(FVector{ 0,0,0 });
}

// Called every frame
void AFormationCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (FVector::Dist(m_vTarget, GetActorLocation()) > m_fAnimationDistance)
		AddMovementInput(m_vTarget - GetActorLocation());
	else
	{
		AddMovementInput({ 0,0,0 });
		m_vTarget = GetActorLocation();
	}
}