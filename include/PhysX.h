#pragma once
#include <PxPhysicsAPI.h>
#include <memory>

using namespace physx;

extern PxFoundation *physicsFoundation;
extern PxPhysics *physics;

template<class T>
struct px_release
{
	void operator() (T* ptr) const
	{
		ptr->release();
	}
};

template<class T>
using px_ptr = std::unique_ptr<T, px_release<T>>;