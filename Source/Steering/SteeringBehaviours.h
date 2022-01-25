#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "UnrealFormations/FormationCharacter.h"

class FSteeringBehaviours : public IModuleInterface
{
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

struct TargetData
{
	FVector Position;
	float Orientation;

	FVector LinearVelocity;
	float AngularVelocity;

	TargetData(FVector position = FVector{ 0,0,0 }, float orientation = 0.f,
		FVector linearVel = FVector{ 0,0,0 }, float angularVel = 0.f) :
		Position(position),
		Orientation(orientation),
		LinearVelocity(linearVel),
		AngularVelocity(angularVel) {}
};

class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual FVector CalculateSteering(float deltaT, AFormationCharacter* pAgent) = 0;

	//Seek Functions
	void SetTarget(const TargetData& target) { m_Target = target; }

protected:
	TargetData m_Target;
};
//****************
//BLENDED STEERING
class BlendedSteering final : public ISteeringBehavior
{
public:
	struct WeightedBehavior
	{
		ISteeringBehavior* pBehavior = nullptr;
		float weight = 0.f;

		WeightedBehavior(ISteeringBehavior* pBehavior, float weight) :
			pBehavior(pBehavior),
			weight(weight)
		{};
	};

	BlendedSteering(TArray<WeightedBehavior> weightedBehaviors) :m_WeightedBehaviors(weightedBehaviors)
	{
	};

	void AddBehaviour(WeightedBehavior weightedBehavior) { m_WeightedBehaviors.Add(weightedBehavior); }
	FVector CalculateSteering(float deltaT, AFormationCharacter* pAgent) override {
		FVector blendedSteering = FVector{};
		auto totalWeight = 0.f;

		for (auto weightedBehavior : m_WeightedBehaviors)
		{
			auto steering = weightedBehavior.pBehavior->CalculateSteering(deltaT, pAgent);
			blendedSteering += weightedBehavior.weight * steering;

			totalWeight += weightedBehavior.weight;
		}

		if (totalWeight > 0.f)
		{
			auto scale = 1.f / totalWeight;
			blendedSteering *= scale;
		}

		return blendedSteering;
	}

	// returns a reference to the weighted behaviors, can be used to adjust weighting. Is not intended to alter the behaviors themselves.
	TArray<WeightedBehavior>& GetWeightedBehaviorsRef() { return m_WeightedBehaviors; }

private:
	TArray<WeightedBehavior> m_WeightedBehaviors = {};
};
//*****************
//PRIORITY STEERING
class PrioritySteering final : public ISteeringBehavior
{
public:
	PrioritySteering(TArray<ISteeringBehavior*> priorityBehaviors)
		:m_PriorityBehaviors(priorityBehaviors)
	{}

	void AddBehaviour(ISteeringBehavior* pBehavior) { m_PriorityBehaviors.Add(pBehavior); }
	FVector CalculateSteering(float deltaT, AFormationCharacter* pAgent) override {
		FVector steering = {};

		for (auto pBehavior : m_PriorityBehaviors)
		{
			steering = pBehavior->CalculateSteering(deltaT, pAgent);
		}

		//If non of the behavior return a valid output, last behavior is returned
		return steering;
	}

private:
	TArray<ISteeringBehavior*> m_PriorityBehaviors = {};
};
///////////////////////////////////////
//SEEK
//****
class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;

	//Seek Behaviour
	FVector CalculateSteering(float deltaT, AFormationCharacter* pAgent) override {
		FVector steering = {};

		steering = m_Target.Position - pAgent->GetActorLocation();
		return steering;
	}
};

///////////////////////////////////////
//FLEE
//****
class Flee : public Seek
{
public:
	Flee() = default;
	virtual ~Flee() = default;

	//Flee Behavior
	FVector CalculateSteering(float deltaT, AFormationCharacter* pAgent) override {
		float distanceToTarget = FVector::Distance(pAgent->GetActorLocation(), m_Target.Position);
		if (distanceToTarget > m_FleeRadius)
		{
			return FVector{ 0,0,0 };
		}

		FVector steering = {};
		steering = Seek::CalculateSteering(deltaT, pAgent);
		steering *= -1;

		return steering;
	}

private:
	float m_FleeRadius = 10.f; // make depending on nr of agents
};