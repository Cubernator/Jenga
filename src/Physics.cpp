#include "Physics.h"
#include "PhysicsScene.h"
#include "GameObject.h"

PxFoundation *physicsFoundation;
PxPhysics *physics;

PhysicsInterface::PhysicsInterface() : m_physicsScene(nullptr), m_pvdConnection(nullptr)
{
	physicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_physicsAllocatorCallback, m_physicsErrorCallback);
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *physicsFoundation, PxTolerancesScale());

	PxInitExtensions(*physics);

	m_physicsCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	PxCudaContextManagerDesc cudaDesc;
	cudaDesc.graphicsDevice = dev;
	cudaDesc.interopMode = PxCudaInteropMode::D3D11_INTEROP;
	m_cudaContextManager = PxCreateCudaContextManager(*physicsFoundation, cudaDesc, nullptr);

#if _DEBUG && USE_PVD
	PxVisualDebuggerConnectionManager * pvdcm = physics->getPvdConnectionManager();
	if (pvdcm) {
		PxVisualDebugger * vd = physics->getVisualDebugger();
		vd->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONSTRAINTS, true);
		vd->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
		vd->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
		PxVisualDebuggerConnectionFlags f = PxVisualDebuggerExt::getAllConnectionFlags();
		m_pvdConnection = PxVisualDebuggerExt::createConnection(pvdcm, "127.0.0.1", 5425, 10, f);
	}
#endif
}

PhysicsInterface::~PhysicsInterface()
{
	if (m_pvdConnection) m_pvdConnection->release();

	m_cudaContextManager->release();
	m_physicsCpuDispatcher->release();
	physics->release();
	physicsFoundation->release();
}

void PhysicsInterface::simulate(float step)
{
	if (m_physicsScene) {
		PxScene * pxs = m_physicsScene->getPhysXObj();
		pxs->simulate(step);
		pxs->fetchResults(true);
	}
}

void PhysicsInterface::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
	PX_UNUSED(constraints);
	PX_UNUSED(count);
}

void PhysicsInterface::onWake(PxActor** actors, PxU32 count)
{
	PX_UNUSED(actors);
	PX_UNUSED(count);
}

void PhysicsInterface::onSleep(PxActor** actors, PxU32 count)
{
	PX_UNUSED(actors);
	PX_UNUSED(count);
}

void PhysicsInterface::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	Collision c;

	GameObject::CollisionCallbackFlags f0 = GameObject::NONE, f1 = GameObject::NONE;

	if (!(pairHeader.flags & PxContactPairHeaderFlag::eREMOVED_ACTOR_0)) {
		c.actor0 = pairHeader.actors[0];
		c.object0 = (GameObject*)c.actor0->userData;
		f0 = c.object0->getCollisionCallbackFlags();
	}
	if (!(pairHeader.flags & PxContactPairHeaderFlag::eREMOVED_ACTOR_1)) {
		c.actor1 = pairHeader.actors[1];
		c.object1 = (GameObject*)c.actor1->userData;
		f1 = c.object1->getCollisionCallbackFlags();
	}

	if ((!f0) && (!f1)) {
		PX_UNUSED(pairs);
		PX_UNUSED(nbPairs);
		return;
	}

	for (PxU32 i = 0; i < nbPairs; ++i) {
		PxU32 contactCount = pairs[i].contactCount;
		if (contactCount) {
			PxContactPairFlags f = pairs[i].flags;
			if (!(f & PxContactPairFlag::eREMOVED_SHAPE_0)) c.shape0 = pairs[i].shapes[0];
			if (!(f & PxContactPairFlag::eREMOVED_SHAPE_1)) c.shape1 = pairs[i].shapes[1];

			c.contacts.resize(contactCount);
			pairs[i].extractContacts(c.contacts.data(), contactCount);

			PxPairFlags pf = pairs[i].events;
			if (pf & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
				if (f0 & GameObject::ENTER) c.object0->onCollisionEnter(c);
				c.flipped = true;
				if (f1 & GameObject::ENTER) c.object1->onCollisionEnter(c);
				c.flipped = false;
			}

			if (pf & PxPairFlag::eNOTIFY_TOUCH_PERSISTS) {
				if (f0 & GameObject::STAY) c.object0->onCollisionStay(c);
				c.flipped = true;
				if (f1 & GameObject::STAY) c.object1->onCollisionStay(c);
				c.flipped = false;
			}

			if (pf & PxPairFlag::eNOTIFY_TOUCH_LOST) {
				if (f0 & GameObject::EXIT) c.object0->onCollisionExit(c);
				c.flipped = true;
				if (f1 & GameObject::EXIT) c.object1->onCollisionExit(c);
				c.flipped = false;
			}
		}
	}
}

void PhysicsInterface::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	PX_UNUSED(pairs);
	PX_UNUSED(count);
}

PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	pairFlags = PxPairFlag::eRESOLVE_CONTACTS
		| PxPairFlag::eDETECT_DISCRETE_CONTACT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		| PxPairFlag::eNOTIFY_TOUCH_LOST
		| PxPairFlag::eNOTIFY_CONTACT_POINTS;
	return PxFilterFlag::eDEFAULT;
}
