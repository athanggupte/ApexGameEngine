#include "pch.h"

#include "Apex/Core/ECS/EntityScript.h"
#include "Apex/Core/ECS/Components.h"
#include "Apex/Graphics/Material.h"

using namespace Apex;

class SCRIPT_TEST_API EntityCreationScript : public EntityScript
{
protected:
	void OnCreate() override
	{
		auto entity = parentScene()->CreateEntity(TEXT("Entity-from-script"));
		auto& meshComp = entity.AddComponent<MeshRendererComponent>();
		meshComp.mesh = GetResourceManager().Get<Mesh>(RESNAME("default-plane"));
		meshComp.material = GetResourceManager().GetOrEmplace(RESNAME("material_from_script"), CreateRef<Material>());

		auto& transform = entity.Transform();
		transform.translation.y += 3.f;
		transform.rotation.x += glm::radians(45.f);
	}
};

APEX_REGISTER_SCRIPT_FACTORY(EntityCreationScript)
