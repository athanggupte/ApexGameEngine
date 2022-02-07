#include "pch.h"

#include "MyComponent.h"
#include "Apex/Application.h"
#include "Apex/Core/Timer.h"
#include "Apex/Core/ResourceManager/ResourceManager.h"
#include "Apex/Core/Log.h"
#include "Apex/Core/ECS/EntityScript.h"
#include "Apex/Core/ECS/ScriptFactory.h"
#include "Apex/Core/ECS/Components.h"
#include "Apex/Graphics/Material.h"
#include "Apex/Utils/Random.h"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace Apex;

class SCRIPT_TEST_API LogScript : public EntityScript
{

public:
	LogScript() = default;

protected:
	
	void OnCreate() override
	{
		APEX_LOG_TRACE("<{0}>::[" __FUNCTION__ "] :: Called...", self().Tag().str());
		if (!self().HasComponent<MeshRendererComponent>()) {
			auto& comp = self().AddComponent<MeshRendererComponent>();
			comp.mesh = GetResourceManager().Get<Mesh>(RESNAME("default-plane"));
			comp.material = GetResourceManager().GetOrEmplace(RESNAME("material_from_script"), CreateRef<Material>());
		}
		auto& meshComp = self().GetComponent<MeshRendererComponent>();
		meshComp.material->SetShader(GetResourceManager().Get<Shader>(RESNAME("shader_StandardPBR")));
		meshComp.material->SetTexture("Albedo", GetResourceManager().AddResource<Texture>(RESNAME("metal_plate_diff_2k"), Texture2D::Create("editor_assets/textures/metal_plate/metal_plate_diff_2k.jpg", true)));
		meshComp.material->SetTexture("Metallic", GetResourceManager().AddResourceFromFile<Texture>(RESNAME("metal_plate_metal_2k"), "editor_assets/textures/metal_plate/metal_plate_metal_2k.jpg"));
		meshComp.material->SetTexture("Roughness", GetResourceManager().AddResourceFromFile<Texture>(RESNAME("metal_plate_rough_2k"), "editor_assets/textures/metal_plate/metal_plate_rough_2k.jpg"));
		meshComp.material->SetTexture("Normal", GetResourceManager().AddResourceFromFile<Texture>(RESNAME("metal_plate_nor_gl_2k"), "editor_assets/textures/metal_plate/metal_plate_nor_gl_2k.jpg"));

		if (!self().HasComponent<MyComponent>())
			self().AddComponent<MyComponent>();

		auto& transform = self().Transform();
		transform.scale = glm::vec3{ 10.0f };
	}

	void OnUpdate(Timestep ts) override
	{
		auto& transform = self().Transform();
		const auto deltaRotation = glm::pi<float>() * ts.GetSeconds() 
							* glm::sin(Timer::GetElapsedTime().GetSeconds());
		transform.rotation += glm::vec3{ 0.f, 1.f, 0.f } * deltaRotation;

		// APEX_LOG_TRACE("<{0}>:: deltaRotation : {1}", self().Tag().str(), deltaRotation);

		/*auto& myComp = self().GetComponent<MyComponent>();
		myComp.detail += Random::Int();
		APEX_LOG_DEBUG("<{0}>::[" __FUNCTION__ "] :: myComp : {1}", self().GetComponent<TagComponent>().tag.str(), myComp.detail);*/

		//if
	}

};

APEX_REGISTER_SCRIPT_FACTORY(LogScript)
