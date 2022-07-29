#include "pch.h"
#include "Apex.h"
#include "Apex/Graphics/Material.h"
#include "Util/Random.h"

using namespace Apex;

class SCRIPT_TEST_API ContactListenerScript : public EntityScript, public IContactListener
{
public:
	ContactListenerScript() = default;
	
protected:
	void OnCreate() override
	{
		// Code to be run once at creation time
		parentScene()->AddContactListener(self(), this);
	}

	void OnUpdate(Timestep ts) override
	{
		// Code to be run every frame
	}
	
	void OnContact(Entity other_entity) override
	{
		if (self().HasComponent<MeshRendererComponent>()) {
			auto& [mesh, material, _] = self().GetComponent<MeshRendererComponent>();
			if (material.IsValid() && material.IsLoaded()) {
				auto randomColor = glm::vec3 { Random::Float(), Random::Float(), Random::Float() } * 5.f;
				material->SetAltColor("Albedo", { randomColor, 1.f });
			}
		}
	}


	void OnOverlapBegin(Entity other_entity) override {}
	void OnOverlapEnd(Entity other_entity) override {}
};

APEX_REGISTER_SCRIPT_FACTORY(ContactListenerScript)