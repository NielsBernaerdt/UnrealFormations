# Formations
Basic strategic group movements/formations implemented in Unreal Engine 4.

## Introduction

As a first ever attempt at tackling C++ in UE4, I opted to expand my knowledge of steering behaviors.
After already having created a 2D flocking behavior using blended steering, I now aspired creating formations inspired by the Total War series.

![Demo](https://media.giphy.com/media/bMlYatFcqInL80NWoI/giphy.gif)

## Implementation
Starting from the top down template provided by UE4, everything is 100% C++. Blueprints are then derived from the base C++ class in order to get everything working in the engine.
### Structure
#### Player controller & player pawn
This is the formation itself. It is responsible for spawning and destroying units, moving the units around as a movement group and switching between different formations.
Every time the player pawn recieves input through the player controller, it will calculate a new formation based on the target location, the current amount of units and the desired formation type.
#### Actor Spawner
Set up as a child object to the player pawn, the actor spawner will take care of spawning and destroying the units. The spawner will also make the player pawn aware of the creation/destruction of one of its units.
#### Unit
A simple unit of the formation. A unit will walk towards the target location using a linear velocity.

## Result
![Final](https://media.giphy.com/media/fyU1sXpyrHy3NG6Z9a/giphy.gif)

## Conclusion
The scope of the subject "Formations" is considerable. Not only the creation of the formation, but also its orientation are important parts in the production of a realistic group movement. Furthermore, the individual orientation of the units as well as the position they sustain relative to the other units, possibly via (blended) steering behaviors, make up to a neat result.

Even though I prioritized the forming of the formation itself while also making sure all agents walk smoothly and coherent relative to the other units of the formation, I still kept those other focus points in my mind. For instance: 

The individual units use a linear velocity to reach their target, here it is perfectly possible to calculate a steering behavior instead of just "seeking" to the target.
The same goes for the general orientation of the formation: instead of just passing the default positions to each unit, I already implemented a function "ApplyRotation". Therefore a rotation calculation can just be pasted there and it will work.

I hoped to implement these myself, and I still will, but I unfortunately did not get to make it before the deadline of this assignment.
## Links
* https://docs.unrealengine.com/4.27/en-US/
* https://en.wikipedia.org/wiki/Triangular_array
* https://www.gamasutra.com/view/feature/3314/coordinated_unit_movement.php
* http://www.spronck.net/pubs/Heijden-CIG2008.pdf
