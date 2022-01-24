#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "FormationCharacter.h"
#include "ActorSpawner.generated.h"

class AFormationCharacter;

UCLASS()
class UNREALFORMATIONS_API AActorSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorSpawner();
	//
	UFUNCTION()
		void SpawnActor();
	UPROPERTY(EditDefaultsOnly, Category = "ActorSpawning")
		TSubclassOf<AFormationCharacter> m_refUnitCharacter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* SpawnVolume;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};