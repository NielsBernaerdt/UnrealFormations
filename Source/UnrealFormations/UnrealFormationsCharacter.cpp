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
			size_t i{};
			for (auto e : m_UnitCharacters)
			{
				e->SetTarget(m_vFormationRotated[i]);
				++i;
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

void AUnrealFormationsCharacter::ConstructLineFormation()
{
	//
	m_fpCurrentFormation = &AUnrealFormationsCharacter::ConstructLineFormation;
	//
	float spacing = 150;
	m_vFormationDefault.Empty();
	FVector pos = GetActorLocation();
	pos.X -= m_UnitCharacters.Num() / 2 * spacing;
	for (auto e : m_UnitCharacters)
	{
		m_vFormationDefault.Add(pos);
		e->SetTarget(pos);
		pos.X += spacing;
	}
	//apply rotation
	ApplyRotation();
}

void AUnrealFormationsCharacter::ConstructCircleFormation()
{
	//
	m_fpCurrentFormation = &AUnrealFormationsCharacter::ConstructCircleFormation;
	//
	if (m_UnitCharacters.Num() == 0)
		return;

	m_vFormationRotated.Empty();
	FVector origin = GetActorLocation();
	float spacing = 150;
	float radius = spacing * m_UnitCharacters.Num() / (2 * PI);
	float angle = 360 / m_UnitCharacters.Num();
	FVector pos = GetActorLocation();



	for (auto e : m_UnitCharacters)
	{
		//e->SetTarget(FVector{ float(pos.X + radius * cos(angle)), float(pos.Y + radius * sin(angle)), 0 });
	
		//UKismetSystemLibrary::DrawDebugPoint(e, { pos.X + radius * float(cos(angle)), pos.Y + radius * float(sin(angle)), e->GetActorLocation().Z }, 10, { 1,1,1 }, 0.3);

		m_vFormationRotated.Add({ pos.X + radius * FMath::Cos(angle * PI / 180), pos.Y + radius * FMath::Sin(angle * PI / 180), 0 });

		angle += 360 / m_UnitCharacters.Num();
	}
}

void AUnrealFormationsCharacter::ConstructTriangleFormation()
{
	m_vFormationDefault.Empty();
	//
	m_fpCurrentFormation = &AUnrealFormationsCharacter::ConstructTriangleFormation;
	//
	if (m_UnitCharacters.Num() < 1)
		return;

	FVector pos = GetActorLocation();
	float spacing = 150;
	float averageY{};

	int currentAgent{ 0 };
	for (size_t row = 1; row < m_UnitCharacters.Num(); ++row)
	{
		float x { 0 - ( float(row) / 2) };
		float y { row * spacing };
		for (size_t column{ 0 }; column < row; ++column)
		{
			m_vFormationDefault.Add(FVector{ pos.X + (x * spacing), pos.Y + y, 0 });
			x += 1;

			++currentAgent;
			if (currentAgent == m_UnitCharacters.Num())
			{
				averageY = row * spacing / 2;
				goto jump;
			}
		}
	}

jump: //here add averageY so that the center point is in the center of the triangle;
	for (auto e : m_vFormationDefault)
	{
		e.Y -= averageY;
	}

	m_vFormationRotated = m_vFormationDefault;
}

void AUnrealFormationsCharacter::ApplyRotation()
{
	m_vFormationRotated.Empty();
	//Rotate
	float angle = (m_vTargetLocation - GetActorLocation()).Rotation().Yaw;
	FVector currentPos{};
	for (auto e : m_vFormationDefault)
	{
		currentPos.X = cos(angle) * (e.X - GetActorLocation().X) - sin(angle) * (e.Y - GetActorLocation().Y) + GetActorLocation().X;
		currentPos.Y = sin(angle) * (e.X - GetActorLocation().X) + cos(angle) * (e.Y - GetActorLocation().Y) + GetActorLocation().Y;

		m_vFormationRotated.Add(currentPos);
	}
}