#pragma once
#include "PhysX.h"
#include <vector>

class GameObject;
class PhysicsScene;

struct Collision
{
	bool flipped;
	GameObject *object0, *object1;
	PxRigidActor *actor0, *actor1;
	PxShape *shape0, *shape1;
	std::vector<PxContactPairPoint> contacts;

	Collision() : flipped(false), object0(nullptr), object1(nullptr), actor0(nullptr), actor1(nullptr), shape0(nullptr), shape1(nullptr) { }

	GameObject * getOtherObject() const { return flipped ? object0 : object1; }
	PxRigidActor * getOtherActor() const { return flipped ? actor0 : actor1; }
	PxShape * getOtherShape() const { return flipped ? shape0 : shape1; }
};

PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);

class PhysicsInterface : public PxSimulationEventCallback
{
private:
	PxDefaultErrorCallback m_physicsErrorCallback;
	PxDefaultAllocator m_physicsAllocatorCallback;
	PxDefaultCpuDispatcher *m_physicsCpuDispatcher;
	PxCudaContextManager *m_cudaContextManager;
	PxVisualDebuggerConnection * m_pvdConnection;

	PhysicsScene * m_physicsScene;

	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;
	void onWake(PxActor** actors, PxU32 count) override;
	void onSleep(PxActor** actors, PxU32 count) override;
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
	void onTrigger(PxTriggerPair* pairs, PxU32 count) override;

public:
	PhysicsInterface();
	~PhysicsInterface();

	template<class SceneType, class... Args>
	SceneType * constructScene(Args&&... args)
	{
		PxSceneDesc desc(physics->getTolerancesScale());
		desc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		desc.cpuDispatcher = m_physicsCpuDispatcher;
		desc.gpuDispatcher = m_cudaContextManager->getGpuDispatcher();
		desc.filterShader = contactReportFilterShader;
		desc.simulationEventCallback = this;
		SceneType * newScene = new SceneType(desc, std::forward<Args>(args)...);
		m_physicsScene = newScene;
		return newScene;
	}

	void simulate(float step);
};