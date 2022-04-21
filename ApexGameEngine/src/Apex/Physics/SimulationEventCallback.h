#pragma once
#include "PhysicsTypes.h"

#include <PxSimulationEventCallback.h>

namespace Apex {
	class Scene;
	class IContactListener;

	class SimulationEventCallback : public physx::PxSimulationEventCallback
	{
	public:
		void onConstraintBreak(physx::PxConstraintInfo* constraints, PxU32 count) override {}
		void onWake(physx::PxActor** actors, PxU32 count) override {}
		void onSleep(physx::PxActor** actors, PxU32 count) override {}
		void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const PxU32 count) override {}
		void onTrigger(physx::PxTriggerPair* pairs, PxU32 count) override {}

		void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, PxU32 nbPairs) override;

		void SetContext(Scene* context_scene);
		void SetContext(const Ref<Scene>& context_scene);
		size_t AddContactListener(IContactListener* listener);

		auto GetContactListener(size_t index) { return m_ContactListeners[index]; }

	protected:
		auto GetContactListeners() { return m_ContactListeners; }

	private:
		std::vector<IContactListener*> m_ContactListeners;
		Scene* m_ContextScene = nullptr;
	};

}
