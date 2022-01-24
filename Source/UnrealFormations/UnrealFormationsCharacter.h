// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UnrealFormationsCharacter.generated.h"

class AFormationCharacter;
class Flee;
class BlendedSteering;

UCLASS(Blueprintable)
class AUnrealFormationsCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AUnrealFormationsCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }
	//
	void AddUnitCharacter(AFormationCharacter* unitCharacter);
	void SetMoveToCursor(bool b) { m_bMoveToCursor = b; }
	void SetCurrentOrientation(FVector orientation) { m_vCurrentOrientation = orientation; }
	// Make functions public and accessible so that it can be accessed in the hud widget blueprint ONLY FOR DEMO PURPOSES
	UFUNCTION(BlueprintCallable, Category = "Formation") void ConstructLineFormation();
	UFUNCTION(BlueprintCallable, Category = "Formation") void ConstructCircleFormation();
	UFUNCTION(BlueprintCallable, Category = "Formation") void ConstructTriangleFormation();
	UFUNCTION(BlueprintCallable, Category = "Formation") void ConstructSquareFormation();
	UFUNCTION(BlueprintCallable, Category = "Formation") void SpawnActors();
	UFUNCTION(BlueprintCallable, Category = "Formation") void DestroyActors();
	UPROPERTY(BlueprintReadWrite, Category = "Debug")
		bool m_bEnableDebug = false;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;
	//
	TArray<AFormationCharacter*> m_UnitCharacters;
	bool m_bMoveToCursor = false;
	FVector m_vTargetLocation;
	TArray<FVector> m_vFormationDefault;
	TArray<FVector> m_vFormationRotated;
	float m_fSpacing = 100;
	FVector m_vCurrentOrientation;

	void ApplyRotation();
	void (AUnrealFormationsCharacter::*m_fpCurrentFormation)() = &AUnrealFormationsCharacter::ConstructLineFormation;

protected:
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
	void GiveUnitsTarget();
};