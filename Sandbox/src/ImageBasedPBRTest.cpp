#include "pch.h"
#include "ImageBasedPBRTest.h"

#include "imgui.h"
#include "Apex/Application.h"
#include "Apex/Core/CameraController.h"
#include "Apex/Core/ECS/Entity.h"
#include "Apex/Core/ECS/Scene.h"
#include "Apex/Core/ECS/SceneSerializer.h"
#include "Apex/Core/ECS/Components/RendererComponents.h"
#include "Apex/Graphics/FBXImporter.h"
#include "Apex/Graphics/Primitives.h"
#include "Apex/Graphics/Renderer/RenderCommands.h"
#include "Apex/Graphics/Renderer/Renderer.h"
#include "Apex/Graphics/RenderPrimitives/Framebuffer.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/Material.h"
#include "Apex/Graphics/PreProcessing/EnvironmentMap.h"

using namespace Apex;

namespace Apex {
	namespace formats {

		struct PackedRGBMA
		{
			/* Packed RGBMA data
			 *	- R : 8 bits (0-255) - Red channel
			 *	- G : 8 bits (0-255) - Green channel
			 *	- B : 8 bits (0-255) - Blue channel
			 *	- M : 7 bits (0-127) - Multiplier channel
			 *	- A : 1 bit  (0-1)   - Alpha, empty or not 
			 */

			union
			{
				uint8_t x;
				uint8_t r;
			};
			union
			{
				uint8_t y;
				uint8_t g;
			};
			union
			{
				uint8_t z;
				uint8_t b;
			};
			uint8_t m : 7;
			uint8_t a : 1;
		};
		static_assert(sizeof(formats::PackedRGBMA) == sizeof(uint32_t));

	}
}

namespace sandbox {

	struct Voxelization
	{
		struct __VoxelUBO
		{
			//formats::PackedRGBMA Color;
			
		};

		Ref<Shader> m_VoxelizationShader;
		Ref<ShaderStorageBuffer> m_ShaderStorageBuffer;

		uint32_t numX, numY, numZ;

		Voxelization(uint32_t nx, uint32_t ny, uint32_t nz)
			: numX(nx), numY(ny), numZ(nz)
		{
			m_VoxelizationShader = Shader::Create("assets/shaders/Voxelization.glsl");
			//m_ShaderStorageBuffer = ShaderStorageBuffer::Create()
		}
	};

	namespace 
	{
		Ref<Scene> s_Scene;
		Ref<Camera> s_Camera;
		Ref<PerspectiveCameraController> s_CameraController;

		Ref<Shader> s_TonemapShader;
		Ref<Shader> s_DebugVertexNormalsShader;

		Ref<Shader> s_SkyboxShader;

		Ref<Texture2D> s_EquirectangularSkyboxTexture;
		Ref<TextureCubemap> s_SkyboxCubemapTexture;
		Ref<TextureCubemap> s_DiffuseIrradianceCubemapTexture;
		Ref<TextureCubemap> s_PrefilteredSpecularCubemapTexture;
		Ref<Texture2D> s_BRDFLUTTexture;

		Ref<Framebuffer> s_HDRFramebuffer;
		Ref<Framebuffer> s_PostprocessFramebuffer;

	}

	namespace options
	{
		bool drawNormals = false;
		bool showIrradianceMap = false;
		bool showPrefilteredSpecularMap = true;
	}

	void SetupScene();
	void DrawSceneNormals();

	bool OnWindowResize(WindowResizeEvent& event);

	ImageBasedPBRTest::ImageBasedPBRTest()
		: Layer("Voxel Test")
	{
		s_Camera = CreateRef<Camera>(Camera::ProjectionType::Perspective,
						Application::Get().GetWindow().GetWidth(),
		                Application::Get().GetWindow().GetHeight());
		s_CameraController = CreateRef<PerspectiveCameraController>(*s_Camera, glm::vec3{-5.f, 5.f, 5.f});
		
		EnvironmentMapPreprocess::Init();
	}


	void ImageBasedPBRTest::OnAttach()
	{
		// Initialize
		s_HDRFramebuffer = Framebuffer::Create({ Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight(), 0 });
		s_HDRFramebuffer->AddColorAttachment(defaults::HDRTextureSpec);

		s_PostprocessFramebuffer = Framebuffer::Create({ Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight(), 0 });
		s_PostprocessFramebuffer->AddColorAttachment(defaults::HDRTextureSpec); // Ping texture
		s_PostprocessFramebuffer->AddColorAttachment(defaults::HDRTextureSpec); // Pong texture

		s_TonemapShader = Shader::Create("internal_assets/shaders/ACESTonemap.glsl");
		s_DebugVertexNormalsShader = Shader::Create("internal_assets/shaders/debugging/DebugVertexNormals.glsl");

		s_SkyboxShader = Shader::Create("internal_assets/shaders/Skybox.glsl");

		Ref<Texture2D> equirectangularSkyboxTexture = Texture2D::Create("assets/textures/christmas_photo_studio_03_2k.hdr", false, true);

		TextureSpec mipmappedEnvMapSpec = {
			TextureAccessFormat::RGB, TextureInternalFormat::RGB16, TextureDataType::FLOAT, TextureFiltering::TRILINEAR
		};
		s_SkyboxCubemapTexture = TextureCubemap::Create(1024u, mipmappedEnvMapSpec, "Cubemap Skybox");
		s_DiffuseIrradianceCubemapTexture = TextureCubemap::Create(32u, defaults::EnvironmentMapSpec, "Diffuse Irradiance");
		s_PrefilteredSpecularCubemapTexture = TextureCubemap::Create(512u, mipmappedEnvMapSpec, "Prefiltered Specular");
		s_BRDFLUTTexture = Texture2D::Create(512u, 512u, { TextureAccessFormat::RG, TextureInternalFormat::RG16, TextureDataType::FLOAT }, "BRDF LUT");

		SetupScene();

		// Misc
		//RenderCommands::SetClearColor(glm::vec4(0.75f, 0.f, 0.75f, 1.f));
		RenderCommands::SetClearColor(glm::vec4(0.f, 0.f, 0.f, 1.f));

		// Preprocess
		EnvironmentMapPreprocess::ConvertEquirectangularToCubemap(equirectangularSkyboxTexture, s_SkyboxCubemapTexture);
		s_SkyboxCubemapTexture->GenerateMipmap();
		EnvironmentMapPreprocess::CreateDiffuseIrradianceMap(s_SkyboxCubemapTexture, s_DiffuseIrradianceCubemapTexture);
		EnvironmentMapPreprocess::CreatePrefilteredSpecularMap(s_SkyboxCubemapTexture, s_PrefilteredSpecularCubemapTexture);
		EnvironmentMapPreprocess::CreateBRDFLUT(s_BRDFLUTTexture);
	}

	void ImageBasedPBRTest::OnUpdate(Timestep ts)
	{
		s_CameraController->OnUpdate(ts);

		// Render to HDR framebuffer
		s_HDRFramebuffer->Bind();
		RenderCommands::SetDepthTest(true);
		RenderCommands::Clear();

		// Draw Scene
		Renderer::SetupLights(s_Scene);
		Renderer::BeginScene(s_CameraController->GetCamera(), s_CameraController->GetTransform());
		s_PrefilteredSpecularCubemapTexture->Bind(TEX_SLOT_ENV_Skybox);
		s_DiffuseIrradianceCubemapTexture->Bind(TEX_SLOT_ENV_Irradiance);
		s_BRDFLUTTexture->Bind(TEX_SLOT_ENV_BRDFLUT);
		s_Scene->Render3D();

		if (options::drawNormals) {
			DrawSceneNormals();
		}

		// Draw Skybox
		{
			s_SkyboxCubemapTexture->Bind(TEX_SLOT_ENV_Skybox);
			if (options::showIrradianceMap)
				s_DiffuseIrradianceCubemapTexture->Bind(TEX_SLOT_ENV_Skybox);
			else if (options::showPrefilteredSpecularMap)
				s_PrefilteredSpecularCubemapTexture->Bind(TEX_SLOT_ENV_Skybox);
			s_SkyboxShader->Bind();
			RenderCommands::SetDepthTestFunction(DepthStencilMode::LEQUAL);
			RenderCommands::Draw(6);
			RenderCommands::SetDepthTestFunction(DepthStencilMode::LESS);
		}

		// Postprocess
		s_HDRFramebuffer->Blit(s_PostprocessFramebuffer);

		// Render tonemapped texture to default framebuffer
		s_PostprocessFramebuffer->Unbind();
		RenderCommands::Clear();

		s_PostprocessFramebuffer->GetColorAttachment(0)->Bind(1);
		s_TonemapShader->Bind();
		RenderCommands::Draw(3);
	}

	void ImageBasedPBRTest::OnEvent(Event& event)
	{
		s_CameraController->OnEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(OnWindowResize);
	}

	void ImageBasedPBRTest::OnImGuiRender()
	{
		ImGui::Begin("Options");
		if (ImGui::Button("Recompile Shaders"))
			Application::Get().GetResourceManager().LoadAll<Shader>();
		ImGui::Checkbox("Draw Normals", &options::drawNormals);

		if (ImGui::Checkbox("Show Irradiance Map", &options::showIrradianceMap)) {
			if (options::showIrradianceMap)
				options::showPrefilteredSpecularMap = false;
		}
		if (ImGui::Checkbox("Show Prefiltered Specular Map", &options::showPrefilteredSpecularMap)) {
			if (options::showPrefilteredSpecularMap)
				options::showIrradianceMap = false;
		}

		static bool albedo_only = false;
		if (ImGui::Checkbox("Albedo Only", &albedo_only)) {
			auto& resourceManager = Application::Get().GetResourceManager();
			if (albedo_only) {
				auto mat = resourceManager.Get<Material>(RESNAME("default-material"));
				mat->SetShader(resourceManager.Get<Shader>(RESNAME("shader_AlbedoUnlit")));
				mat->SetTexture("Albedo", resourceManager.Get<Texture>(RESNAME("texture_UV-Checker")));
			}
			else {
				auto mat = resourceManager.Get<Material>(RESNAME("default-material"));
				mat->SetShader(resourceManager.Get<Shader>(RESNAME("shader_StandardPBR")));
				mat->SetTexture("Albedo", resourceManager.Get<Texture>(RESNAME("texture_UV-Checker")));
			}
		}

		ImGui::End();
	}

	bool OnWindowResize(WindowResizeEvent& event)
	{
		if (event.GetWidth() > 0 && event.GetHeight() > 0) {
			s_HDRFramebuffer->Resize(event.GetWidth(), event.GetHeight());
			s_Scene->OnViewportResize(event.GetWidth(), event.GetHeight());
			s_CameraController->OnResize(event.GetWidth(), event.GetHeight());
		}

		return false;
	}


	// Additional function definitions
	void SetupScene()
	{
		// Load resources
		{
			auto& resourceManager = Application::Get().GetResourceManager();
			resourceManager.Insert<Shader>(RESNAME("shader_StandardPBR"), "internal_assets/shaders/StandardPBR_new.glsl");
			resourceManager.Insert<Shader>(RESNAME("shader_AlbedoUnlit"), "internal_assets/shaders/AlbedoUnlit.glsl");
			resourceManager.Insert<Shader>(RESNAME("shader_SkyboxReflectionUnlit"), "assets/shaders/SkyboxReflectionUnlit.glsl");

			resourceManager.Insert<Texture>(RESNAME("texture_UV-Checker"), "internal_assets/textures/UV_Checker.png");

			resourceManager.Insert<Mesh>(RESNAME("default-plane"), Primitives::Plane::GetMesh());
			resourceManager.Insert<Mesh>(RESNAME("default-cube"), Primitives::Cube::GetMesh());
			resourceManager.Insert<Mesh>(RESNAME("default-sphere"), FBXImporter::LoadMesh(FileSystem::GetFileIfExists("assets/models/sphere-tris-1.fbx"), "Sphere"));
			resourceManager.Insert<Mesh>(RESNAME("heavy-metal-squiggle-orb"), FBXImporter::LoadMesh(FileSystem::GetFileIfExists("assets/models/heavy-metal-squiggle-orb-4k/source/bool_low-tris.fbx")));
			resourceManager.Insert<Mesh>(RESNAME("oldcar"), FBXImporter::LoadMesh(FileSystem::GetFileIfExists("assets/models/old-rusty-car/oldcar.fbx")));

			{
			Resource<Material> mat = resourceManager.Insert<Material>(
				RESNAME("default-material"),
				CreateRef<Material>(resourceManager.Get<Shader>(RESNAME("shader_StandardPBR"))));
			//mat->SetTexture("Albedo", resourceManager.Get<Texture>(RESNAME("texture_UV-Checker")));
			}

			{
			Resource<Material> mat = resourceManager.Insert<Material>(
				RESNAME("unlit-reflection-material"),
				CreateRef<Material>(resourceManager.Get<Shader>(RESNAME("shader_SkyboxReflectionUnlit"))));
			}

			{
			Resource<Material> mat = resourceManager.Insert<Material>(
				RESNAME("heavy-metal-squiggle-orb.mat"),
				CreateRef<Material>(resourceManager.Get<Shader>(RESNAME("shader_StandardPBR"))));
			mat->SetTexture("Albedo",
				resourceManager.Insert<Texture>(
					RESNAME("heavy-metal-squiggle-orb.tex_albedo"),
					Texture2D::Create(
						"assets/models/heavy-metal-squiggle-orb-4k/textures/bool_low_bool_t_BaseColor.png",
						true)));
			mat->SetTexture("Metallic",
				resourceManager.Insert<Texture>(
					RESNAME("heavy-metal-squiggle-orb.tex_metallic"),
					"assets/models/heavy-metal-squiggle-orb-4k/textures/bool_low_bool_t_Metallic.png"));
			mat->SetTexture("Normal",
				resourceManager.Insert<Texture>(
					RESNAME("heavy-metal-squiggle-orb.tex_normal"),
					"assets/models/heavy-metal-squiggle-orb-4k/textures/bool_low_bool_t_NormalGL.png"));
			mat->SetTexture("Roughness",
				resourceManager.Insert<Texture>(
					RESNAME("heavy-metal-squiggle-orb.tex_roughness"),
					"assets/models/heavy-metal-squiggle-orb-4k/textures/bool_low_bool_t_Roughness.png"));
			mat->SetTexture("AO",
				resourceManager.Insert<Texture>(
					RESNAME("heavy-metal-squiggle-orb.tex_ao"),
					"assets/models/heavy-metal-squiggle-orb-4k/textures/bool_low_bool_t_AO.png"));
			}

			{
				Resource<Material> mat = resourceManager.Insert<Material>(
					RESNAME("oldcar-material"),
					CreateRef<Material>(resourceManager.Get<Shader>(RESNAME("shader_StandardPBR"))));
				mat->SetTexture("Albedo",
					resourceManager.Insert<Texture>(
						RESNAME("oldcar.tex_albedo"),
						Texture2D::Create(
							"assets/models/old-rusty-car/car_d.png",
							true)));
				mat->SetTexture("Metallic",
					resourceManager.Insert<Texture>(
						RESNAME("oldcar.tex_metallic"),
						"assets/models/old-rusty-car/car_m.png"));
				mat->SetTexture("Normal",
					resourceManager.Insert<Texture>(
						RESNAME("oldcar.tex_normal"),
						"assets/models/old-rusty-car/car_n.png"));
				mat->SetTexture("Roughness",
					resourceManager.Insert<Texture>(
						RESNAME("oldcar.tex_roughness"),
						"assets/models/old-rusty-car/car_r.png"));
			}

			resourceManager.LoadAllResources();
		}

		// Load scene
		s_Scene = Apex::CreateRef<Scene>();
		auto serializer = SceneSerializerFactory().SetFormat(SceneSerializerFactory::Format::XML).Build(s_Scene);
		if (serializer->Deserialize("assets/scenes/LightsScene.xml")) {
			s_Scene->OnSetup();
		} else {
			Application::Get().Close();
		}

		{
			Entity oldcar = s_Scene->CreateEntity(TEXT("oldcar"));
			MeshRendererComponent& meshComp = oldcar.AddComponent<MeshRendererComponent>();
			meshComp.mesh = Application::Get().GetResourceManager().Get<Mesh>(RESNAME("oldcar"));
			meshComp.material = Application::Get().GetResourceManager().Get<Material>(RESNAME("oldcar-material"));
			oldcar.Transform().rotation.x = glm::radians(-90.f);
			oldcar.Transform().scale *= 0.025f;
		}
	}

	void DrawSceneNormals()
	{
		auto view = s_Scene->View<TransformComponent, MeshRendererComponent>();
		view.each([](auto entity, const TransformComponent& transform, MeshRendererComponent& mesh_renderer_component) {
			if (mesh_renderer_component.mesh.IsValid()) {
				Renderer::Submit(s_DebugVertexNormalsShader, mesh_renderer_component.mesh->GetVAO(), transform.GetTransform());
			}
		});
	}


}
