#pragma once

// Forward Declarations
namespace physx
{
	class PxPhysics;
	class PxScene;
}

namespace Apex {
	class SimulationEventCallback;

	//using PhysicsScene = std::shared_ptr<physx::PxScene>;
	using PhysicsScene = physx::PxScene*;

	class PhysicsManager
	{
	public:
		static void Init();
		static PhysicsScene CreateScene();
		static void Shutdown();

		static physx::PxPhysics& GetPhysics();

	private:
		static SimulationEventCallback& GetSimulationEventCallback();

		friend class Scene;
	};

}
