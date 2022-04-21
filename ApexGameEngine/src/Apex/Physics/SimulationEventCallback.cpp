#include "apex_pch.h"
#include "SimulationEventCallback.h"

#include "Apex/Core/ECS/Entity.h"
#include "Apex/Core/ECS/Scene.h"
#include "Apex/Core/ECS/Components/PhysicsComponents.h"

#include <PxRigidActor.h>

namespace Apex {

	void SimulationEventCallback::onContact(const physx::PxContactPairHeader& pairHeader,
	                                        const physx::PxContactPair* pairs, PxU32 nbPairs)
	{
		APEX_CORE_ASSERT(m_ContextScene, "Context scene is NULL! Set context scene before starting simulation.");

		for (uint32_t i = 0; i < nbPairs; ++i) {
			const physx::PxContactPair& pair = pairs[i];
			if (pair.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
				auto entityId0 = static_cast<entt::entity>(reinterpret_cast<uintptr_t>(pairHeader.actors[0]->userData));
				auto entityId1 = static_cast<entt::entity>(reinterpret_cast<uintptr_t>(pairHeader.actors[1]->userData));
				Entity entity0 { entityId0, m_ContextScene }, entity1 { entityId1, m_ContextScene };

				if (entity0.HasComponent<ContactListenerIdComponent>()) {
					auto& index = entity0.GetComponent<ContactListenerIdComponent>().listenerId;
					APEX_CORE_ASSERT(m_ContactListeners.size() > index, "Invalid ContactListener index");
					if (auto listener = m_ContactListeners[index]) {
						listener->OnContact(entity1);
					}
				} else if (entity1.HasComponent<ContactListenerIdComponent>()) {
					auto& index = entity1.GetComponent<ContactListenerIdComponent>().listenerId;
					APEX_CORE_ASSERT(m_ContactListeners.size() > index, "Invalid ContactListener index");
					if (auto listener = m_ContactListeners[index]) {
						listener->OnContact(entity0);
					}
				}
			}
		}
	}

	void SimulationEventCallback::SetContext(Scene* context_scene)
	{
		m_ContextScene = context_scene;
	}

	void SimulationEventCallback::SetContext(const Ref<Scene>& context_scene)
	{
		m_ContextScene = context_scene.get();
	}

	size_t SimulationEventCallback::AddContactListener(IContactListener* listener)
	{
		m_ContactListeners.push_back(listener);
		return m_ContactListeners.size();
	}
}
