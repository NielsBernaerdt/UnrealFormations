// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealFormationsCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
//
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ActorSpawner.h"
#include "FormationCharacter.h"
#include "Steering/SteeringBehaviours.h"

AUnrealFormationsCharacter::AUnrealFormationsCharacter()
{
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

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AUnrealFormationsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction("SpawnActors", IE_Released, this, &AUnrealFormationsCharacter::SpawnActors);
	PlayerInputComponent->BindAction("DestroyActors", IE_Released, this, &AUnrealFormationsCharacter::DestroyActors);
	PlayerInputComponent->BindAction("LineFormation", IE_Released, this, &AUnrealFormationsCharacter::ConstructLineFormation);
	PlayerInputComponent->BindAction("CircleFormation", IE_Released, this, &AUnrealFormationsCharacter::ConstructCircleFormation);
	PlayerInputComponent->BindAction("TriangleFormation", IE_Released, this, &AUnrealFormationsCharacter::ConstructTriangleFormation);
	PlayerInputComponent->BindAction("SquareFormation", IE_Released, this, &AUnrealFormationsCharacter::ConstructSquareFormation);
}

void AUnrealFormationsCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (CursorToWorld != nullptr)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);

			if (m_bMoveToCursor == true)
			{
				m_vTargetLocation = TraceHitResult.Location;
				(this->*m_fpCurrentFormation)();
			}

		}
	}
}

void AUnrealFormationsCharacter::SpawnActors()
{
	//Find the Actor Spawner in the world, and invoke it's Spawn Actor function
	AActor* ActorSpawnerTofind = UGameplayStatics::GetActorOfClass(GetWorld(), AActorSpawner::StaticClass());

	AActorSpawner* ActorSpawnerReference = Cast<AActorSpawner>(ActorSpawnerTofind);
	if (ActorSpawnerReference)
	{
		ActorSpawnerReference->SpawnActor();
		(this->*m_fpCurrentFormation)();
	}
}

void AUnrealFormationsCharacter::DestroyActors()
{
	if (m_UnitCharacters.Num() < 1)
		return;
	//Destroy the last added actor
	AActor* actor = m_UnitCharacters.Last();
	m_UnitCharacters.RemoveAt(m_UnitCharacters.Num()-1);
	actor->Destroy();
	(this->*m_fpCurrentFormation)();
}

void AUnrealFormationsCharacter::AddUnitCharacter(AFormationCharacter* unitCharacter)
{
	m_UnitCharacters.Add(unitCharacter);
}

void AUnrealFormationsCharacter::GiveUnitsTarget()
{
	size_t i{};
	for (auto e : m_UnitCharacters)
	{
		e->SetTarget(m_vFormationRotated[i]);
		++i;
	}
}

void AUnrealFormationsCharacter::ConstructLineFormation()
{
	//
	m_fpCurrentFormation = &AUnrealFormationsCharacter::ConstructLineFormation;
	if (m_UnitCharacters.Num() < 1)
		return;
	m_vFormationDefault.Empty();
	//
	int agentsPerLine{ 5 };
	FVector cPos = m_vTargetLocation;
	cPos.X -= agentsPerLine / 2.0 * m_fSpacing;

	for (size_t row{0}; row < (m_UnitCharacters.Num() / agentsPerLine); ++row)
	{
		for (size_t column{0}; column < agentsPerLine; ++column)
		{
			m_vFormationDefault.Add(
				{
					cPos.X + column * m_fSpacing,
					cPos.Y + row * m_fSpacing,
					cPos.Z
				});
		}
	}
	int agentsLeft{ m_UnitCharacters.Num() % agentsPerLine };
	cPos = m_vTargetLocation;
	cPos.X -= agentsLeft / 2.0 * m_fSpacing;
	cPos.Y += (m_UnitCharacters.Num() / agentsPerLine) * m_fSpacing;
	for (size_t extra{ 0 }; extra < agentsLeft; ++extra)
	{
		m_vFormationDefault.Add(
			{
				cPos.X + extra * m_fSpacing,
				cPos.Y,
				cPos.Z
			});
	}
	//Translate formationcenter to actual center
	float avgY = abs(m_vFormationDefault.Last().Y - m_vFormationDefault[0].Y) / 2;
	for (FVector& e : m_vFormationDefault)
	{
		e.Y -= avgY;
	}


	ApplyRotation();
	GiveUnitsTarget();
}

void AUnrealFormationsCharacter::ConstructCircleFormation()
{
	//
	m_fpCurrentFormation = &AUnrealFormationsCharacter::ConstructCircleFormation;
	if (m_UnitCharacters.Num() == 0)
		return;
	m_vFormationRotated.Empty();
	//
	FVector cPos = m_vTargetLocation;
	float radius = m_fSpacing * m_UnitCharacters.Num() / (2 * PI);
	float angle = 360 / m_UnitCharacters.Num();

	for (auto e : m_UnitCharacters)
	{
		m_vFormationRotated.Add({ cPos.X + radius * FMath::Cos(angle * PI / 180), cPos.Y + radius * FMath::Sin(angle * PI / 180), cPos.Z });
		angle += 360 / m_UnitCharacters.Num();
	}

	GiveUnitsTarget();
}

void AUnrealFormationsCharacter::ConstructTriangleFormation()
{
	//
	m_fpCurrentFormation = &AUnrealFormationsCharacter::ConstructTriangleFormation;
	if (m_UnitCharacters.Num() < 1)
		return;
	m_vFormationDefault.Empty();
	//
	FVector pos = m_vTargetLocation;
	int currentAgent{ 0 };
	float maxRow = (-1 + FMath::Sqrt(1 + 8 * m_UnitCharacters.Num())) / 2;  //calculate the row of an index in a triangular array
	maxRow = ceil(maxRow);
	for (size_t row = 1; row < size_t(maxRow) + 1; ++row)
	{
		float x { 0 - ( float(row) / 2) };
		float y { row * m_fSpacing };
		for (size_t column{ 0 }; column < row; ++column)
		{
			m_vFormationDefault.Add(FVector{ pos.X + (x * m_fSpacing),  pos.Y + y, pos.Z });
			x += 1;

			++currentAgent;
			if (currentAgent == m_UnitCharacters.Num())
			{
				goto jump;
			}
		}
	}

jump: //Translate formationcenter to actual center
	float avgY = abs(m_vFormationDefault.Last().Y - m_vFormationDefault[0].Y) / 2;
	for (FVector& e : m_vFormationDefault)
	{
		e.X += m_fSpacing / 2;
		e.Y -= m_fSpacing;
		e.Y -= avgY;
	}

	m_vFormationRotated = m_vFormationDefault;
	GiveUnitsTarget();
}

void AUnrealFormationsCharacter::ConstructSquareFormation()
{
	//
	m_fpCurrentFormation = &AUnrealFormationsCharacter::ConstructSquareFormation;
	if (m_UnitCharacters.Num() < 4)
	{
		ConstructCircleFormation();
		m_fpCurrentFormation = &AUnrealFormationsCharacter::ConstructSquareFormation;
		return;
	}
	m_vFormationDefault.Empty();
	//Calculate side length:
	int nrAgentsExtra = m_UnitCharacters.Num() % 4;
	FVector cPos = m_vTargetLocation;
	float sideLength{};
	if (nrAgentsExtra != 0)
		sideLength = (m_UnitCharacters.Num() / 4 + 1) * m_fSpacing;
	else
		sideLength = (m_UnitCharacters.Num() / 4) * m_fSpacing;
	int agentsOnThisSide{};
	FVector currentPos{};
	//Upperside
	if (nrAgentsExtra != 0)
	{
		--nrAgentsExtra;
		agentsOnThisSide = m_UnitCharacters.Num() / 4 + 1;
	}
	else
		agentsOnThisSide = m_UnitCharacters.Num() / 4 ;
	currentPos = { cPos.X - sideLength / 2, cPos.Y + sideLength / 2, cPos.Z };
	for (size_t i{}; i < agentsOnThisSide; ++i)
	{
		m_vFormationDefault.Add(currentPos);
		currentPos.X += sideLength / agentsOnThisSide;
	}
	//RightSide
	if (nrAgentsExtra != 0)
	{
		--nrAgentsExtra;
		agentsOnThisSide = m_UnitCharacters.Num() / 4 + 1;
	}
	else
		agentsOnThisSide = m_UnitCharacters.Num() / 4;
	currentPos = { cPos.X + sideLength / 2, cPos.Y + sideLength / 2, cPos.Z };
	for (size_t i{}; i < agentsOnThisSide; ++i)
	{
		m_vFormationDefault.Add(currentPos);
		currentPos.Y -= sideLength / agentsOnThisSide;
	}
	//LowerSide
	if (nrAgentsExtra != 0)
	{
		--nrAgentsExtra;
		agentsOnThisSide = m_UnitCharacters.Num() / 4 + 1;
	}
	else
		agentsOnThisSide = m_UnitCharacters.Num() / 4;
	currentPos = { cPos.X + sideLength / 2, cPos.Y - sideLength / 2, cPos.Z };
	for (size_t i{}; i < agentsOnThisSide; ++i)
	{
		m_vFormationDefault.Add(currentPos);
		currentPos.X -= sideLength / agentsOnThisSide;
	}
	//LeftSide
	if (nrAgentsExtra != 0)
	{
		--nrAgentsExtra;
		agentsOnThisSide = m_UnitCharacters.Num() / 4 + 1;
	}
	else
		agentsOnThisSide = m_UnitCharacters.Num() / 4;
	currentPos = { cPos.X - sideLength / 2, cPos.Y - sideLength / 2, cPos.Z };
	for (size_t i{}; i < agentsOnThisSide; ++i)
	{
		m_vFormationDefault.Add(currentPos);
		currentPos.Y += sideLength / agentsOnThisSide;
	}

	m_vFormationRotated = m_vFormationDefault;
	GiveUnitsTarget();
}

void AUnrealFormationsCharacter::ApplyRotation() //Point rotation over center point of formation
{
	m_vFormationRotated = m_vFormationDefault;

	//m_vFormationRotated.Empty();

	//float angle = ((GetActorLocation() + GetActorForwardVector()) - GetActorLocation()).Rotation().Yaw - (m_vTargetLocation - GetActorLocation()).Rotation().Yaw;
	////Fix some of the possible angle values
	//if (angle < -180)
	//	angle += 360;
	//else if (angle > 180)
	//	angle -= 360;
	//angle = angle * PI / 180;
	////
	//FVector cPos = GetActorLocation();
	//FVector currentPos{};
	////
	//
	//for (auto point : m_vFormationDefault)
	//{
	//	currentPos.X = cos(angle) * (point.X - cPos.X) - sin(angle) * (point.Y - cPos.Y) + cPos.X;

	//	currentPos.Y = sin(angle) * (point.X - cPos.X) + cos(angle) * (point.Y - cPos.Y) + cPos.Y;

	//	m_vFormationRotated.Add(currentPos);
	//}
}