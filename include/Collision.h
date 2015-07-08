#pragma once

#include "PhysX.h"
#include <vector>

class GameObject;

struct Collision
{
	bool flipped;
	GameObject *object0, *object1;
	PxRigidActor *actor0, *actor1;
	PxShape *shape0, *shape1;
	std::vector<PxContactPairPoint> contacts;

	Collision() : flipped(false), object0(nullptr), object1(nullptr), actor0(nullptr), actor1(nullptr), shape0(nullptr), shape1(nullptr) {}

	GameObject * getOtherObject() const { return flipped ? object0 : object1; }
	PxRigidActor * getOtherActor() const { return flipped ? actor0 : actor1; }
	PxShape * getOtherShape() const { return flipped ? shape0 : shape1; }
};
