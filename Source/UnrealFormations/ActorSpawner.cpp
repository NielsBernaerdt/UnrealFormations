#include "ActorSpawner.h"
//
#include "Components/BoxComponent.h"
#include "UnrealFormationsCharacter.h"
#include "FormationCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AActorSpawner::AActorSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	SpawnVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AActorSpawner::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AActorSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AActorSpawner::SpawnActor()
{
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();

	if (AUnrealFormationsCharacter* MyPawn = Cast<AUnrealFormationsCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
	{
		MyPawn->AddUnitCharacter(GetWorld()->SpawnActor<AFormationCharacter>(m_refUnitCharacter, SpawnLocation, SpawnRotation));
	}
}