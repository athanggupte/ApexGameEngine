#pragma once

// Forward Declarations
namespace physx
{
	class PxMaterial;
	class PxRigidActor;
	class PxPhysics;
	class PxScene;
}

namespace Apex {

	struct BoxCollider;
	struct TransformComponent;
	struct RigidBodyComponent;
	struct SphereCollider;

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


		static physx::PxRigidActor* CreatePhysicsActor(TransformComponent& transform, RigidBodyComponent& rb, BoxCollider& box_collider, physx::PxMaterial* pxMaterial);
		static physx::PxRigidActor* CreatePhysicsActor(TransformComponent& transform, RigidBodyComponent& rb, SphereCollider& sphere_collider, physx::PxMaterial* pxMaterial);

	private:
		static SimulationEventCallback& GetSimulationEventCallback();

		friend class Scene;
	};


}
