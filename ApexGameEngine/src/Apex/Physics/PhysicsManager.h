#pragma once
// #include <PxScene.h>

namespace physx
{
	class PxPhysics;
	class PxScene;
}

namespace Apex {

	// class PhysicsScene;
	using PhysicsScene = std::shared_ptr<physx::PxScene>;

	class PhysicsManager
	{
	public:
		static void Init();
		static PhysicsScene CreateScene();
		static void Shutdown();

		static physx::PxPhysics& GetPhysics();
	};


	/*class PhysicsScene
	{
	public:
		PhysicsScene() = default;
		PhysicsScene(physx::PxScene* scene) ;
		~PhysicsScene();

		physx::PxScene* operator -> () { return m_PxScene; }
		const physx::PxScene* operator -> () const { return m_PxScene; }

		physx::PxScene* Get() const { return m_PxScene; }

	private:
		physx::PxScene* m_PxScene = nullptr;
	};*/

}
