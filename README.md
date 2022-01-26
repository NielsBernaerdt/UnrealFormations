# Formations
Basic strategic group movements/formations implemented in Unreal Engine 4

## Introduction

As a first ever attempt at tackling C++ in UE4, I opted to expand my knowledge of steering behaviors.
After already having created a 2D flocking behavior using blended steering, I now desired creating formations inspired by the Total War series.

## Demo

## Implementation
Starting from the top down template provided by UE4, everything is 100% C++. Blueprints are then made derived from the base C++ class in order to get everything working in the engine.
### Structure
#### Player controller & player pawn
The formation itself. It is responsible for spawning and destroying units, moving the units around as a formation and switching between different formations.
Every time the player pawn recieves input through the player conroller, it will calculate a formation based on the target location, the current ammount of units and the desired formation type
#### Actor Spawner
Set up as a child object to the player pawn, the actor spawner will take care of spawning and destroying the units.
#### Unit
A simple unit making part of the formation. Will walk towards the target location if it hasn't reached it yet.

## Result
A fundamental project with ...


## Conclusion
The scope of "Formations" is considerable. Even though I prioritized the forming of the formation itself and making sure all agents walk smoothly & coherent relative to the other units of the formation, I still kept other focus points like moving the agents using a blended steering behavior, or the orientation of the whole formation in my mind.
The base unit takes a target and uses a linear velocity to move towards a target, thus a (blended) steering behavior can just be calculated and passed on instead. The same goes for the orientation of the formation: instead of just passing the default locations to each unit, I already implemented a function "ApplyRotation". In that function a rotation can be implemented and the units will use it by default.

I hoped to implement these myself, and I still will, but I unfortunately did not get to make it before the deadline of this assignment.
## Links
https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/SpawnAndDestroyActors/
https://en.wikipedia.org/wiki/Triangular_array
https://www.gamasutra.com/view/feature/3314/coordinated_unit_movement.php
http://www.spronck.net/pubs/Heijden-CIG2008.pdf
