#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FormationCharacter.generated.h"

UCLASS()
class UNREALFORMATIONS_API AFormationCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFormationCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//
	void SetTarget(const FVector& v) { m_vTarget = v; }

private:
	FVector m_vTarget;
	const float m_fAnimationDistance = 100;
};