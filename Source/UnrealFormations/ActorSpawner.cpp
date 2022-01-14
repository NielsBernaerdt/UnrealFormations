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

	UObject* SpawnActor = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Game/TopDownCPP/Blueprints/BP_FormationCharacter")));
	UBlueprint* GeneratedBP = Cast<UBlueprint>(SpawnActor);

	if (AUnrealFormationsCharacter* MyPawn = Cast<AUnrealFormationsCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
	{
		MyPawn->AddUnitCharacter(GetWorld()->SpawnActor<AActor>(GeneratedBP->GeneratedClass, SpawnLocation, SpawnRotation));
	}
}