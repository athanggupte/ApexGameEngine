#include "apex_pch.h"
#include "PhysicsTypes.h"
#include "PhysicsManager.h"

#include "SimulationEventCallback.h"

#include <PxPhysicsAPI.h>

namespace Apex {

	struct PhysXData
	{
		inline static physx::PxDefaultAllocator sDefaultAllocator;
		inline static physx::PxDefaultErrorCallback sDefaultErrorCallback;

		physx::PxFoundation* mFoundation = nullptr;
		physx::PxPvd* mPvd = nullptr;
		physx::PxPhysics* mPhysics = nullptr;
		physx::PxDefaultCpuDispatcher* mCpuDispatcher = nullptr;

		std::vector<std::unique_ptr<physx::PxScene, std::function<void(physx::PxScene*)>>> mScenes;

		Unique<SimulationEventCallback> mSimulationEventCallback = nullptr;

		~PhysXData()
		{
			mScenes.clear();
			mCpuDispatcher->release();
			mPhysics->release();
			if (mPvd) {
				auto transport = mPvd->getTransport();
				mPvd->release();
				transport->release();
			}
			PxCloseExtensions();
			mFoundation->release();
		}
	};

	physx::PxFilterFlags DefaultFilterShader(physx::PxFilterObjectAttributes attributes0,
	                                        physx::PxFilterData filterData0,
	                                        physx::PxFilterObjectAttributes attributes1,
	                                        physx::PxFilterData filterData1,
	                                        physx::PxPairFlags& pairFlags, const void* constantBlock,
	                                        PxU32 constantBlockSize);

	static Ref<PhysXData> s_PhysXData;

	void PhysicsManager::Init()
	{
		s_PhysXData = CreateRef<PhysXData>();

		// Create Foundation
		s_PhysXData->mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, PhysXData::sDefaultAllocator, PhysXData::sDefaultErrorCallback);
		APEX_CORE_ASSERT(s_PhysXData->mFoundation, "Physx :: Foundation could NOT be initialized!");

		// Create Pvd and Connect to local PVD instance
		s_PhysXData->mPvd = PxCreatePvd(*s_PhysXData->mFoundation);
		physx::PxPvdTransport* mTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		s_PhysXData->mPvd->connect(*mTransport, physx::PxPvdInstrumentationFlag::eALL);

		// Create Tolerance scale
		physx::PxTolerancesScale mScale;
		mScale.length = 1.f;
		mScale.speed = 9.81f;

		// Create Physics
		s_PhysXData->mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *s_PhysXData->mFoundation, mScale, true, s_PhysXData->mPvd);
		APEX_CORE_ASSERT(s_PhysXData->mPhysics, "PxPhysics could NOT be initialized!");

		APEX_CORE_ASSERT(PxInitExtensions(*s_PhysXData->mPhysics, s_PhysXData->mPvd), "PhysXExtensions could NOT be initialized!");

		// Create Dispatchers
		s_PhysXData->mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

		// Create Simulation Event Callback
		s_PhysXData->mSimulationEventCallback = CreateUnique<SimulationEventCallback>();
	}

	PhysicsScene PhysicsManager::CreateScene()
	{
		physx::PxSceneDesc sceneDesc(s_PhysXData->mPhysics->getTolerancesScale());
		sceneDesc.gravity = { 0.f, -9.81f, 0.f };
		//sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		sceneDesc.filterShader = DefaultFilterShader;
		sceneDesc.simulationEventCallback = s_PhysXData->mSimulationEventCallback.get();
		sceneDesc.cpuDispatcher = s_PhysXData->mCpuDispatcher;
		sceneDesc.flags.set(physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS);

		// physx::PxScene* scene = s_PhysXData->mPhysics->createScene(sceneDesc);
		auto deleter = [](physx::PxScene* scene) {
			scene->release();
		};
		auto& scene = s_PhysXData->mScenes.emplace_back(s_PhysXData->mPhysics->createScene(sceneDesc), deleter);

		// Set PVD scene client flags
		if (physx::PxPvdSceneClient* pvdClient = scene->getScenePvdClient()) {
			pvdClient->setScenePvdFlags(
				physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS | physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS |
				physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES);
		}


		return scene.get();
	}

	void PhysicsManager::Shutdown()
	{
		s_PhysXData.reset();
	}

	physx::PxPhysics& PhysicsManager::GetPhysics()
	{
		return *s_PhysXData->mPhysics;
	}

	SimulationEventCallback& PhysicsManager::GetSimulationEventCallback()
	{
		return *s_PhysXData->mSimulationEventCallback;
	}


	physx::PxFilterFlags DefaultFilterShader(physx::PxFilterObjectAttributes attributes0,
	                                         physx::PxFilterData filterData0,
	                                         physx::PxFilterObjectAttributes attributes1,
	                                         physx::PxFilterData filterData1,
	                                         physx::PxPairFlags& pairFlags,
	                                         const void* constantBlock, PxU32 constantBlockSize)
	{
		using namespace physx;
		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1)) {
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlag::eDEFAULT;
		}

		pairFlags = PxPairFlag::eCONTACT_DEFAULT;

		pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS
		           | PxPairFlag::eNOTIFY_TOUCH_FOUND
		           | PxPairFlag::eNOTIFY_TOUCH_LOST;

		return PxFilterFlag::eDEFAULT;
	}

}
