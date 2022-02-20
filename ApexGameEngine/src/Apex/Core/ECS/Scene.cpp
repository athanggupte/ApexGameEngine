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
		return CreateEntity(HASH("Unnamed Entity"));
	}

	Entity Scene::CreateEntity(StringHandle name)
	{
		return CreateEntityWithGUID(GenerateGUID(), name);
	}

	Entity Scene::CreateEntityWithGUID(const GUID& guid)
	{
		return CreateEntityWithGUID(guid, HASH("Unnamed Entity"));
	}

	Entity Scene::CreateEntityWithGUID(const GUID& guid, StringHandle name)
	{
		auto entity = Entity{ m_Registry.create(), this };
		entity.AddComponent<IDComponent>(guid);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name);
		return entity;
	}

	void Scene::RemoveEntity(Entity entity)
	{
		m_Registry.destroy(entity.m_EntityId);
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

	void Scene::OnPlay()
	{
		auto& resourceManager = Application::Get().GetResourceManager();

		m_Registry.view<NativeScriptComponent>()
			.each([this](auto entity, NativeScriptComponent& nsc) {
				nsc.instance = nsc.factory->InstantiateScript();
				nsc.instance->m_Entity = Entity{ entity, this };
				nsc.instance->OnCreate();
			});
		
		OnSetup();
		resourceManager.LoadAll<Texture>();
		resourceManager.LoadAll<Shader>();
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
				Renderer2D::DrawGlyphs(textRenderer.text, transform.GetTransform(), textRenderer.color, textRenderer.font);
			});
	}

	void Scene::Render3D()
	{
		const auto view = m_Registry.view<TransformComponent, MeshRendererComponent>();
		view.each([&](auto entity, const TransformComponent& transform, MeshRendererComponent& mesh_component) {
			if (mesh_component.mesh.IsValid() && mesh_component.material.IsValid()) {
				if (const auto shader = mesh_component.material->GetShader(); shader.IsValid()) {
					mesh_component.material->Bind();
					Renderer::Submit(shader.Get(), mesh_component.mesh->GetVAO(), transform.GetTransform());
				} else {
					Renderer::Submit(errorShader, mesh_component.mesh->GetVAO(), transform.GetTransform());
				}
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

	using AllBuiltInComponents = ComponentGroup<TransformComponent, SpriteRendererComponent, CameraComponent,
	                                            NativeScriptComponent, TextRendererComponent, MeshRendererComponent>;

	Ref<Scene> Scene::Copy()
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		auto& dstRegistry = newScene->m_Registry;
		auto& srcRegistry = m_Registry;

		std::unordered_map<GUID, entt::entity> enttMap;

		auto v = srcRegistry.view<IDComponent>();
		for (entt::entity e : v) {
			GUID id = m_Registry.get<IDComponent>(e).id;
			auto tag = m_Registry.get<TagComponent>(e).tag;
			auto entity = newScene->CreateEntityWithGUID(id, tag);
			enttMap.emplace(id, static_cast<entt::entity>(entity));
		}

		CopyComponent(AllBuiltInComponents{}, srcRegistry, dstRegistry, enttMap);

		//CopyComponent<TransformComponent>(srcRegistry, dstRegistry, enttMap);
		//CopyComponent<SpriteRendererComponent>(srcRegistry, dstRegistry, enttMap);
		//CopyComponent<CameraComponent>(srcRegistry, dstRegistry, enttMap);
		//CopyComponent<NativeScriptComponent>(srcRegistry, dstRegistry, enttMap);
		//CopyComponent<MeshRendererComponent>(srcRegistry, dstRegistry, enttMap);

		return newScene;
	}

	void Scene::OnUpdate(Timestep ts)
	{
		// Physics Update
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

	void Scene::OnEditorUpdate(Timestep ts)
	{
		Render3D();
		Render2D();
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		Options.ViewportWidth = width;
		Options.ViewportHeight = height;
		
		m_Registry.view<CameraComponent>().each([this](auto& cameraComponent) {
			if (!cameraComponent.fixedAspectRatio)
				cameraComponent.camera.SetViewportSize(Options.ViewportWidth, Options.ViewportHeight);
		});
	}
	
	void Scene::SetPrimaryCamera(const Entity& entity)
	{
		Options.PrimaryCamera = entity.m_EntityId;
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
