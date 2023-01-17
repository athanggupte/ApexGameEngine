#include <apex_pch.h>

#include <utility>
#include "Scene.h"

#include "ScriptFactory.h"
#include "Apex/Application.h"
#include "Apex/Core/Camera.h"
#include "Apex/Core/ECS/Entity.h"
#include "Apex/Core/ECS/EntityScript.h"
#include "Apex/Core/ECS/Components.h"

#include "Apex/Graphics/Renderer/Renderer.h"
#include "Apex/Graphics/Renderer/Renderer2D.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Graphics/Mesh.h"
#include "Apex/Graphics/Material.h"

// PhysX includes
#include "Apex/Graphics/Renderer/TextRenderer.h"
#include "Apex/Physics/SimulationEventCallback.h"

#include <PxPhysicsAPI.h>

namespace Apex {

	static Ref<Shader> errorShader;

	Scene::Scene()
	{
		// (void)m_Registry.group<TransformComponent, SpriteRendererComponent>();
	
		if (!errorShader) {
			constexpr auto vertexSource = R"(
			#version 450 core

			layout(location = ATTRIB_LOC_Position) in vec3 a_Position;

			layout(location = 0) uniform Camera {
				vec3 u_CameraPosition;
				mat4 u_ViewProjection;
			};
			layout(location = 1) uniform mat4 u_Model;

			layout(location = 0) out vec3 v_Position;

			void main() {
				gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
			}
			)";

			constexpr auto fragmentSource = R"(
			#version 450 core

			layout(location = 0) out vec4 o_Color;

			layout(location = 0) in vec3 v_Position;

			void main() {
				o_Color = vec4(0.976, 0.2, 0.949, 1.0);
			}
			)";

			errorShader = Shader::Create("ErrorShader", vertexSource, fragmentSource);
		}
	}
	
	Entity Scene::CreateEntity()
	{
		// static uint32_t counter = 1;
		return CreateEntity(TEXT("Unnamed Entity"));
	}

	Entity Scene::CreateEntity(StringHandle name)
	{
		return CreateEntityWithGUID(GenerateGUID(), name);
	}

	Entity Scene::CreateEntityWithGUID(const GUID& guid)
	{
		return CreateEntityWithGUID(guid, TEXT("Unnamed Entity"));
	}

	Entity Scene::CreateEntityWithGUID(const GUID& guid, StringHandle name)
	{
		APEX_CORE_ASSERT(m_GuidRegistry.count(guid) == 0, "Entity with given GUID already exists!");

		auto entity = Entity{ m_Registry.create(), this };
		entity.AddComponent<IDComponent>(guid);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name);

		m_GuidRegistry.emplace(guid, entity.m_EntityId);

		return entity;
	}

	Entity Scene::CreateMetaEntity()
	{
		auto entity = Entity{ m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		return entity;
	}

	void Scene::RemoveEntity(Entity entity)
	{
		m_Registry.destroy(entity.m_EntityId);
	}

	Entity Scene::GetEntityByGUID(const GUID& guid)
	{
		return { m_GuidRegistry[guid], this };
	}

	std::vector<Entity> Scene::GetEntitiesByTag(StringHandle tag)
	{
		return {};
	}

	void Scene::OnSetup()
	{
		auto& resourceManager = Application::Get().GetResourceManager();

		m_Registry.view<SpriteRendererComponent>()
			.each([&resourceManager](SpriteRendererComponent& sprite) {
				if (sprite.texture.IsValid() && !sprite.texture.IsLoaded())
					resourceManager.Load(sprite.texture.GetId());
			});

		m_Registry.view<MeshRendererComponent>()
			.each([&resourceManager](MeshRendererComponent& meshComp) {
				if (meshComp.mesh.IsValid() && !meshComp.mesh.IsLoaded())
					resourceManager.Load(meshComp.mesh.GetId());
				if (meshComp.material.IsValid() && !meshComp.material.IsLoaded())
					resourceManager.Load(meshComp.material.GetId());
			});
	}

	// TODO: Change place to put this
	struct __AddedToPhysxTag{};

	// TODO: Remove this conditioning it upon some variable
	physx::PxMaterial* pxMaterial = nullptr;

	void Scene::OnPlay()
	{
		auto& resourceManager = Application::Get().GetResourceManager();

		m_Registry.view<NativeScriptComponent>()
			.each([this](auto entity, NativeScriptComponent& nsc) {
			#ifndef APEX_DIST
				nsc.instance = nsc.factory->CloneScript(nsc.editorInstance);
			#else
				nsc.instance = nsc.factory->InstantiateScript();
			#endif
				nsc.instance->m_Entity = Entity{ entity, this };
				nsc.instance->OnCreate();
			});
		
		OnSetup();
		resourceManager.LoadAll<Texture>();
		resourceManager.LoadAll<Shader>();

		PhysicsManager::GetSimulationEventCallback().SetContext(this);
		m_PhysicsScene = PhysicsManager::CreateScene();

		pxMaterial = PhysicsManager::GetPhysics().createMaterial(0.8f, 0.4f, 0.6f);

		//physx::PxRigidStatic* groundPlane = PxCreatePlane(m_PhysicsScene->getPhysics(), physx::PxPlane{ 0.f, 1.f, 0.f, 0.f }, *pxMaterial);
		//m_PhysicsScene->addActor(*groundPlane);

		m_Registry.view<TransformComponent, RigidBodyComponent, BoxCollider>()
			.each([this](auto entity, TransformComponent& transform, RigidBodyComponent& rb, BoxCollider& box_collider) {
				auto actor = PhysicsManager::CreatePhysicsActor(transform, rb, box_collider, pxMaterial);
				actor->userData = reinterpret_cast<void*>(static_cast<uintptr_t>(entity));
				APEX_CORE_INFO("Adding Physx::RigidBody for entity : {} ({})", entity, m_Registry.get<TagComponent>(entity).tag.str());
				m_PhysicsScene->addActor(*actor);
				m_Registry.emplace<__AddedToPhysxTag>(entity);
			});

		m_Registry.view<TransformComponent, RigidBodyComponent, SphereCollider>()
			.each([this](auto entity, TransformComponent& transform, RigidBodyComponent& rb, SphereCollider& sphere_collider) {
				auto actor = PhysicsManager::CreatePhysicsActor(transform, rb, sphere_collider, pxMaterial);
				actor->userData = reinterpret_cast<void*>(static_cast<uintptr_t>(entity));
				APEX_CORE_INFO("Adding Physx::RigidBody for entity : {} ({})", entity, m_Registry.get<TagComponent>(entity).tag.str());
				m_PhysicsScene->addActor(*actor);
				m_Registry.emplace<__AddedToPhysxTag>(entity);
			});

		m_PhysicsTimeAccumulator = 0.f;
	}

	void Scene::OnStop()
	{
		auto& resourceManager = Application::Get().GetResourceManager();

		m_Registry.view<NativeScriptComponent>()
			.each([this](auto entity, NativeScriptComponent& nsc) {
				nsc.instance->OnDestroy();
				nsc.factory->DestroyScript(nsc.instance);
				nsc.instance = nullptr;
			});

		m_PhysicsScene = {};
		m_PhysicsTimeAccumulator = 0.f;
	}

	void Scene::OnUpdate(Timestep ts)
	{
		// Physics Update
		m_PhysicsTimeAccumulator += ts.GetSeconds();
		if (m_PhysicsTimeAccumulator >= m_PhysicsTimestep) {
			OnPhysicsUpdate(ts);
		}

		uint32_t nbActiveActors = 0;
		auto activeActors = m_PhysicsScene->getActiveActors(nbActiveActors);
		for (auto i = 0u; i < nbActiveActors; ++i) {
			auto actor = (physx::PxRigidActor*)activeActors[i];
			auto entity = static_cast<entt::entity>(reinterpret_cast<intptr_t>(actor->userData));
			auto& transform = m_Registry.get<TransformComponent>(entity);
			auto pxTransform = actor->getGlobalPose();
			transform.translation.x = pxTransform.p.x;
			transform.translation.y = pxTransform.p.y;
			transform.translation.z = pxTransform.p.z;
			glm::quat q { pxTransform.q.w, pxTransform.q.x, pxTransform.q.y, pxTransform.q.z };
			transform.rotation = glm::eulerAngles(q);
		}

		// Update Pathfinding
		// Update AI
		// Update Scripts
		m_Registry.view<NativeScriptComponent>()
			.each([this, ts](NativeScriptComponent& nsc) {
				nsc.instance->OnUpdate(ts);
			});
		// Triggers
		// Sound
		// Render
		/*if (Options.PrimaryCamera != entt::null) {
			auto [camera, transform] = m_Registry.get<CameraComponent, TransformComponent>(Options.PrimaryCamera);
			Renderer2D::BeginScene(camera.camera, transform.GetTransform());
			Render2D();
			Renderer2D::EndScene();
		}*/
		// PostProcess
	}

	void Scene::OnPhysicsUpdate(Timestep ts)
	{
		m_PhysicsScene->simulate(m_PhysicsTimestep);
		m_PhysicsScene->fetchResults(true);
		m_PhysicsTimeAccumulator -= m_PhysicsTimestep;

		m_Registry.view<NativeScriptComponent>()
			.each([this, ts](NativeScriptComponent& nsc) {
				nsc.instance->OnPhysicsUpdate(m_PhysicsTimestep);
			});

		m_Registry.view<TransformComponent, RigidBodyComponent, BoxCollider>(entt::exclude<__AddedToPhysxTag>)
			.each([this](auto entity, TransformComponent& transform, RigidBodyComponent& rb, BoxCollider& box_collider) {
				//Entity e{entity, this};
				//APEX_LOG_DEBUG("new PhysxEntity w/ BoxCollider ({})", e.Tag().str());
				auto actor = PhysicsManager::CreatePhysicsActor(transform, rb, box_collider, pxMaterial);
				actor->userData = reinterpret_cast<void*>(static_cast<uintptr_t>(entity));
				APEX_CORE_INFO("Adding Physx::RigidBody for entity : {} ({})", entity, m_Registry.get<TagComponent>(entity).tag.str());
				m_PhysicsScene->addActor(*actor);
				m_Registry.emplace<__AddedToPhysxTag>(entity);
			});

		m_Registry.view<TransformComponent, RigidBodyComponent, SphereCollider>(entt::exclude<__AddedToPhysxTag>)
			.each([this](auto entity, TransformComponent& transform, RigidBodyComponent& rb, SphereCollider& sphere_collider) {
				//Entity e{entity, this};
				//APEX_LOG_DEBUG("new PhysxEntity w/ SphereCollider ({})", e.Tag().str());
				auto actor = PhysicsManager::CreatePhysicsActor(transform, rb, sphere_collider, pxMaterial);
				actor->userData = reinterpret_cast<void*>(static_cast<uintptr_t>(entity));
				APEX_CORE_INFO("Adding Physx::RigidBody for entity : {} ({})", entity, m_Registry.get<TagComponent>(entity).tag.str());
				m_PhysicsScene->addActor(*actor);
				m_Registry.emplace<__AddedToPhysxTag>(entity);
			});
	}

	void Scene::Render2D()
	{
		// IMP: When iterating components owned by a group outside the group donot add new
		//      entities with the same list of components -> iterators get invalidated

		const auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
		view.each([](const TransformComponent& transform, const SpriteRendererComponent& sprite) {
			if (sprite.visible) {
				if (sprite.useTexture) {
					if (sprite.texture.IsValid() && sprite.texture.IsLoaded()) {
						Renderer2D::DrawQuad(transform.GetTransform(), std::dynamic_pointer_cast<Texture2D>(sprite.texture.Get()), sprite.tilingFactor);
					} else {
						Renderer2D::DrawQuad(transform.GetTransform(), { 0.95f, 0.2f, 0.98f, 1.0f });
					}
				}
				else
					Renderer2D::DrawQuad(transform.GetTransform(), sprite.color);
			}
		});

		m_Registry.view<TransformComponent, TextRendererComponent>()
			.each([](const TransformComponent& transform, const TextRendererComponent& textRenderer) {
				TextRenderer::DrawGlyphs(textRenderer.text, transform.GetTransform(), textRenderer.font, textRenderer.color, textRenderer.width, textRenderer.thickness);
			});
	}

	void Scene::Render3D()
	{
		const auto view = m_Registry.view<TransformComponent, MeshRendererComponent>();
		view.each([&](auto entity, const TransformComponent& transform, MeshRendererComponent& mesh_component) {
			if (mesh_component.mesh.IsValid() && mesh_component.material.IsValid() && mesh_component.material->GetShader()->IsValid()) {
				mesh_component.material->Bind();
				Renderer::Submit(mesh_component.material->GetShader().Get(), mesh_component.mesh->GetVAO(), transform.GetTransform());
			} else if (mesh_component.mesh.IsValid()) {
				Renderer::Submit(errorShader, mesh_component.mesh->GetVAO(), transform.GetTransform());
			} else {
				GUID& id = m_Registry.get<IDComponent>(entity).id;
				APEX_CORE_ERROR("[Entity : {}] Mesh not found!", Strings::Get(id.Hash()));
			}
		});
	}

	template<typename Component_t>
	static constexpr char const* dbgComponentName()
	{
		if constexpr (std::is_same_v<Component_t, TransformComponent>)
			return "TransformComponent";
		if constexpr (std::is_same_v<Component_t, SpriteRendererComponent>)
			return "SpriteRendererComponent";
		if constexpr (std::is_same_v<Component_t, CameraComponent>)
			return "CameraComponent";
		if constexpr (std::is_same_v<Component_t, NativeScriptComponent>)
			return "NativeScriptComponent";
		if constexpr (std::is_same_v<Component_t, TextRendererComponent>)
			return "TextRendererComponent";
		if constexpr (std::is_same_v<Component_t, MeshRendererComponent>)
			return "MeshRendererComponent";
		if constexpr (std::is_same_v<Component_t, LightComponent>)
			return "LightComponent";
		if constexpr (std::is_same_v<Component_t, RigidBodyComponent>)
			return "RigidBodyComponent";
		if constexpr (std::is_same_v<Component_t, BoxCollider>)
			return "BoxCollider";
		if constexpr (std::is_same_v<Component_t, SphereCollider>)
			return "SphereCollider";


		if constexpr (std::is_same_v<Component_t, __AddedToPhysxTag>)
			return "__AddedToPhysxTag";


		return "Unknown Component Type";
	}

	template<typename... Component_t>
	static void CopyComponent(entt::registry& src, entt::registry& dst, const std::unordered_map<GUID, entt::entity>& enttMap)
	{
		([&]() {
			auto view = src.view<Component_t>();
			for (auto srcEntt : view)
			{
				auto id = src.get<IDComponent>(srcEntt).id;
				auto dstEntt = enttMap.at(id);

				auto& component = src.get<Component_t>(srcEntt);
				dst.emplace_or_replace<Component_t>(dstEntt, component);

				APEX_CORE_INFO("Added {} to entity {}", dbgComponentName<Component_t>(), Strings::Get(id.Hash()));
			}
		}(), ...);
	}

	template<typename... Component_t>
	struct ComponentGroup {};

	template<typename... Component_t>
	static void CopyComponent(ComponentGroup<Component_t...>, entt::registry& src, entt::registry& dst, const std::unordered_map<GUID, entt::entity>& enttMap)
	{
		CopyComponent<Component_t...>(src, dst, enttMap);
	}

	using AllBuiltInComponents = ComponentGroup<TransformComponent, SpriteRendererComponent, CameraComponent, LightComponent,
	                                            NativeScriptComponent, TextRendererComponent, MeshRendererComponent,
	                                            RigidBodyComponent, BoxCollider, SphereCollider>;

	using MetaComponents = ComponentGroup<ContactListenerIdComponent>;

	Ref<Scene> Scene::Copy()
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		auto& dstRegistry = newScene->m_Registry;
		auto& srcRegistry = m_Registry;

		std::unordered_map<GUID, entt::entity> newSceneEntityMap;

		auto v = srcRegistry.view<IDComponent>();
		for (entt::entity e : v) {
			GUID id = m_Registry.get<IDComponent>(e).id;
			auto tag = m_Registry.get<TagComponent>(e).tag;
			auto entity = newScene->CreateEntityWithGUID(id, tag);
			newSceneEntityMap.emplace(id, static_cast<entt::entity>(entity));
		}

		CopyComponent(AllBuiltInComponents{}, srcRegistry, dstRegistry, newSceneEntityMap);

		CopyComponent(MetaComponents{}, srcRegistry, dstRegistry, newSceneEntityMap);

		return newScene;
	}

	void Scene::OnEditorUpdate(Timestep ts)
	{
		Render3D();
		Render2D();
	}

	void Scene::OnScriptRender(Timestep ts)
	{
		m_Registry.view<NativeScriptComponent>()
			.each([ts](NativeScriptComponent& nsc) {
				nsc.instance->OnRender(ts);
			});
	}

	//void Scene::OnImGuiRender()
	//{
	//	m_Registry.view<NativeScriptComponent>()
	//		.each([](NativeScriptComponent& nsc) {
	//			nsc.instance->OnImGuiRender();
	//		});
	//}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_Options.viewportWidth = width;
		m_Options.viewportHeight = height;
		
		m_Registry.view<CameraComponent>().each([this](auto& cameraComponent) {
			if (!cameraComponent.fixedAspectRatio)
				cameraComponent.camera.SetViewportSize(m_Options.viewportWidth, m_Options.viewportHeight);
		});
	}
	
	void Scene::SetPrimaryCamera(const Entity& entity)
	{
		m_Options.primaryCamera = entity.m_EntityId;
	}

	void Scene::AddContactListener(Entity entity, IContactListener* listener)
	{
		auto index = PhysicsManager::GetSimulationEventCallback().AddContactListener(listener);
		m_Registry.emplace<ContactListenerIdComponent>(entity, index-1);
	}

	//template<typename Component_t>
	//void Scene::OnComponentAdded(Entity entity, Component_t& component)
	//{
	//}
	
	//template<>
	//void Scene::OnComponentAdded(Entity entity, TagComponent& component)
	//{
	//}
	
	//template<>
	//void Scene::OnComponentAdded(Entity entity, TransformComponent& component)
	//{
	//}
	
	//template<> __declspec(dllexport)
	//void Scene::OnComponentAdded(Entity entity, CameraComponent& component)
	//{
	//	if (Options.ViewportWidth > 0 && Options.ViewportHeight > 0)
	//		component.camera.SetViewportSize(Options.ViewportWidth, Options.ViewportHeight);
	//}
	
	//template<>
	//void Scene::OnComponentAdded(Entity entity, SpriteRendererComponent& component)
	//{
	//}
	
	//template<>
	//void Scene::OnComponentAdded(Entity entity, NativeScriptComponent& component)
	//{
	//}

	//template<>
	//void Scene::OnComponentAdded(Entity entity, MeshRendererComponent& component)
	//{
	//}
	
}
