#include <Apex.h>
#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Apex/Material/Material.h"
#include "Apex/Model/Model.h"
#include "Apex/Physics/ParticleSystem/ParticleSystem2D.h"

#include "Games/DXBall/DXBall.h"

class SandboxLayer : public Apex::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_SquareColor(5.78f, 3.23f, 3.47f, 1.0f), m_BlurEffect()
	{
		/// Square Data ///
		m_SquareVA = Apex::VertexArray::Create();

		Apex::Ref<Apex::VertexBuffer> squareVB;
		float squareVertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		squareVB = Apex::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
			{ Apex::ShaderDataType::Float3, "a_Position" },
			{ Apex::ShaderDataType::Float2, "a_TexCoord" }
		});
		m_SquareVA->AddVertexBuffer(squareVB);

		Apex::Ref<Apex::IndexBuffer> squareIB;
		uint32_t squareIndices[] = {
			0, 1, 2,
			0, 2, 3
		};
		squareIB = Apex::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		m_SquareVA->AddIndexBuffer(squareIB);

		m_SquareVA->Unbind();
		/// End Square Data ///

		/// Instances Data ///
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.075f, 0.075f, 0.075f));

		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
					m_InstancedModelMatrices.push_back(translate(glm::mat4(1.0f), glm::vec3(i*0.125f - 9.5f * 0.125f, j*0.125f - 9.5f * 0.125f, -2.3f)) * scale);
			}
		}

		/*for (glm::mat4 mat : m_InstancedModelMatrices)
		{
			APEX_TRACE("{0}", Apex::MathParser::ParseMatrix(mat));
		}*/

		/// Texture Shader ///
		
		auto textureShader = Apex::AssetManager::GetShaderLibrary().Load("assets/shaders/Texture.glsl");
		m_PusheenTexture = Apex::Texture2D_HDR::Create("assets/textures/pusheen-thug-life.png");
		m_CheckerTexture = Apex::Texture2D_HDR::Create("assets/textures/Checkerboard.png");
		textureShader->Bind();
		textureShader->SetUniInt("u_Texture", 0);

		/* ------------------TEST CODE------------------------- */
		//Apex::Material material;
		//material.SetShader(textureShader);
		//material.AddTexture("Texture", m_CheckerTexture);
		//material.Bind();
		/* ---------------------------------------------------- */

		/// Flat Shader ///
		std::string flatVertexSrc = R"(
			#version 450

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Model[100];

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Model[gl_InstanceID] * vec4(a_Position, 1.0);
			}
		)";

		std::string flatFragmentSrc = R"(
			#version 450

			layout(location = 0) out vec4 o_Color;

			uniform vec4 u_Color;
			
			in vec3 v_Position;

			void main()
			{
				o_Color = u_Color;
			}
		)";

		m_FlatShader = Apex::Shader::Create("FlatColorShader", flatVertexSrc, flatFragmentSrc);
		m_FlatShader->Bind();
		m_FlatShader->SetUniFloat4("u_Color", m_SquareColor);
		//m_FlatShader->SetUniMat4("u_ModelTemp", glm::translate(glm::mat4(1.0f), glm::vec3(-9.5f * 0.085f, -9.5f * 0.085f, -2.3f)) * scale);

		/// Frame Buffer ///
		m_FrameBuffer = Apex::FrameBuffer::Create(true);
		m_ScreenColorTexture = Apex::Texture2D_HDR::Create();
		m_FrameBuffer->AttachTexture(m_ScreenColorTexture);

		m_PostProcessFB = Apex::FrameBuffer::Create(false);
		m_BrightColorTexture = Apex::Texture2D_HDR::Create();
		m_PostProcessFB->AttachTexture(m_BrightColorTexture);

		m_ScreenVA = Apex::VertexArray::Create();
		Apex::Ref<Apex::VertexBuffer> screenVB;
		float screenVertices[] = {
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
		
			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};
		screenVB = Apex::VertexBuffer::Create(screenVertices, sizeof(screenVertices));
		screenVB->SetLayout({
			{ Apex::ShaderDataType::Float2, "a_Position" },
			{ Apex::ShaderDataType::Float2, "a_TexCoord" }
			});
		m_ScreenVA->AddVertexBuffer(screenVB);

		m_BrightShader = Apex::Shader::Create("assets/shaders/BrightnessSplit.glsl");
		m_BrightShader->Bind();
		m_BrightShader->SetUniInt("u_ScreenTexture", 0);
		
		m_BlendShader = Apex::Shader::Create("assets/shaders/Blend.glsl");
		m_BlendShader->Bind();
		m_BlendShader->SetUniInt("u_SceneTexture", 0);
		m_BlendShader->SetUniInt("u_BlurTexture", 1);

		auto screenHDRShader = Apex::AssetManager::GetShaderLibrary().Load("assets/shaders/ScreenHDR.glsl");
		screenHDRShader->Bind();
		screenHDRShader->SetUniInt("u_ScreenTexture", 0);
		auto screenShader = Apex::AssetManager::GetShaderLibrary().Load("assets/shaders/Screen.glsl");
		screenShader->Bind();
		screenShader->SetUniInt("u_ScreenTexture", 0);
	}

	// Inherited via Layer
	void OnAttach() override {}

	void OnDetach() override {}

	void OnUpdate() override
	{
		auto[mouseX, mouseY] = Apex::Input::GetMousePos();
		if (!ImGui::IsAnyItemActive() && !ImGui::IsAnyWindowFocused()) {
			std::pair<float, float> mouseDiff = {
				 Apex::Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_LEFT) ? (mouseX - m_MousePos.first) : 0.0f,
				 Apex::Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_LEFT) ? (mouseY - m_MousePos.second) : 0.0f
			};
			m_CameraRotation += Apex::Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_RIGHT) ? ((mouseY - m_MousePos.second) * m_CameraRotateSpeed * Apex::Timer::GetSeconds()) : 0.0f;
			m_CameraPosition.x -= mouseDiff.first * m_CameraMoveSpeed;
			m_CameraPosition.y += mouseDiff.second * m_CameraMoveSpeed;
		}
		
		m_MousePos = { mouseX, mouseY };

		m_MsgList.clear();
		std::stringstream ss;
		ss << "mousePos : " << mouseX << " , " << mouseY;
		m_MsgList.emplace_back(ss.str());
		ss.str(std::string());
		ss << "Camera Position : " << m_CameraPosition.x << " , " << m_CameraPosition.y << " , " << m_CameraPosition.z;
		m_MsgList.emplace_back(ss.str());

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);


		//// First Pass : Render to custom framebuffer ////
		m_FrameBuffer->Bind();
		Apex::RenderCommands::SetDepthTest(true);
		Apex::RenderCommands::SetClearColor({ 0.12f, 0.1185f, 0.12f, 1.0f });
		Apex::RenderCommands::Clear();
		Apex::Renderer::BeginScene(m_Camera);

		/*static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.075f, 0.075f, 0.075f));
		m_FlatShader->Bind();
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 20; j++) {
				m_FlatShader->SetUniFloat4("u_Color", m_SquareColor);
				Apex::Renderer::Submit(m_FlatShader, m_SquareVA,
					glm::translate(glm::mat4(1.0f), glm::vec3(i*0.085f - 9.5f * 0.085f, j*0.085f - 9.5f * 0.085f, -2.3f)) * scale);
			}
		}*/
		
		/*glm::mat4 matArray[4] = {
			glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, -2.3f)),
			glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, -2.3f)),
			glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, -2.3f)),
			glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, -2.3f))
		};*/

		Apex::Renderer::SubmitArray(m_FlatShader, m_SquareVA, m_InstancedModelMatrices.data(), 100 /*m_InstancedModelMatrices.size()*/);
		

		auto textureShader = Apex::AssetManager::GetShaderLibrary().GetShader("Texture");

		m_CheckerTexture->Bind();
		Apex::Renderer::Submit(textureShader, m_SquareVA, glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, 0.1f, -1.7f)));
		m_PusheenTexture->Bind();
		Apex::Renderer::Submit(textureShader, m_SquareVA, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));

		Apex::Renderer::EndScene();

		//// Split Pass : Render to Bright Texture ////
		m_PostProcessFB->Bind();
		Apex::RenderCommands::SetDepthTest(false);
		Apex::RenderCommands::SetClearColor({ 0.12f, 0.1185f, 0.12f, 1.0f });
		Apex::RenderCommands::Clear();
		m_ScreenColorTexture->Bind(0);
		Apex::Renderer::SubmitPostProcess(m_BrightShader, m_ScreenVA);

		//// Blur Pass : Render to Blur framebuffer ////
		
		auto blurTex = m_BlurEffect.GaussianBlur(5, m_BrightColorTexture, m_ScreenVA);

		//// Final Pass : Render to default framebuffer ////
		m_FrameBuffer->Unbind();
		Apex::RenderCommands::SetDepthTest(false);
		Apex::RenderCommands::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		Apex::RenderCommands::Clear();
		m_ScreenColorTexture->Bind(0);
		//m_BrightColorTexture->Bind(0);
		blurTex->Bind(1);
		//Apex::AssetManager::GetShaderLibrary().GetShader("Screen");
		//auto screenShader = Apex::AssetManager::GetShaderLibrary().GetShader("ScreenHDR");
		//screenShader->Bind();
		//screenShader->SetUniFloat1("exposure", m_Exposure);
		//screenShader->SetUniFloat1("contrast", m_Contrast);
		m_BlendShader->Bind();
		m_BlendShader->SetUniFloat1("exposure", m_Exposure);
		Apex::Renderer::SubmitPostProcess(m_BlendShader, m_ScreenVA);

	}

	void OnImGuiRender() override
	{
		ImGui::ShowMetricsWindow();
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::DragFloat("Exposure", &m_Exposure, 0.005f, 0.0f, 10.0f);
		ImGui::DragFloat("Contrast", &m_Contrast, 0.005f, 0.0f, 3.0f);
		for (auto& msg : m_MsgList)
			ImGui::TextUnformatted(msg.c_str());
		ImGui::End();
	}

	void OnEvent(Apex::Event& event) override
	{
		Apex::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<Apex::MouseScrolledEvent>(APEX_BIND_EVENT_FN(SandboxLayer::OnMouseScrolled));
	}

	bool OnMouseScrolled(Apex::MouseScrolledEvent& event)
	{
		m_CameraPosition.z -= event.GetOffsetY();
		
		return false;
	}

private:
	/// Main scene objects ///
	Apex::Ref<Apex::Shader> m_FlatShader;
	//Apex::Ref<Apex::Shader> m_TextureShader;
	Apex::Ref<Apex::VertexArray> m_SquareVA;
	Apex::Ref<Apex::Texture2D_HDR> m_PusheenTexture;
	Apex::Ref<Apex::Texture2D_HDR> m_CheckerTexture;

	/// Custom framebuffer objects ///
	Apex::Ref<Apex::VertexArray> m_ScreenVA;
	Apex::Ref<Apex::Shader> m_BrightShader;
	Apex::Ref<Apex::Shader> m_BlendShader;
	Apex::Ref<Apex::Texture2D_HDR> m_ScreenColorTexture;
	Apex::Ref<Apex::Texture2D_HDR> m_BrightColorTexture;
	Apex::Ref<Apex::FrameBuffer> m_FrameBuffer;
	Apex::Ref<Apex::FrameBuffer> m_PostProcessFB;

	Apex::Blur m_BlurEffect;

	/// Camera ///
	Apex::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraRotation = 0.0f;
	float m_CameraMoveSpeed = 0.0025f;
	float m_CameraRotateSpeed = 30.0f;

	std::pair<float, float> m_MousePos = { 0.0f, 0.0f };
	
	glm::vec4 m_SquareColor;

	std::list<std::string> m_MsgList;

	std::vector<glm::mat4> m_InstancedModelMatrices;

	float m_Exposure = 1.0f;
	float m_Contrast = 0.0f;
};

class ModelLayer : public Apex::Layer
{
public:
	ModelLayer()
		: Layer("Model"), m_Camera(63.5f, 16.f / 9.f), m_LightCamera(-150.f, 150.f, -150.f, 150.f, 100.0f, 410.0f)
	{
		m_Camera.SetPosition({0.f, 0.f, 10.f});

		m_Model = Apex::Model::LoadModel(m_ModelFilePath);
		m_Shader = Apex::Shader::Create("assets/shaders/PBR.glsl");
		m_Shader->Bind();
		m_Shader->SetUniInt("u_TextureAlbedo",    0);
		m_Shader->SetUniInt("u_TextureNormal",    1);
		m_Shader->SetUniInt("u_TextureMetallic",  2);
		m_Shader->SetUniInt("u_TextureRoughness", 3);
		//m_Texture = Apex::Texture2D::Create("assets/models/Baseball/Baseball_diffuse.jpg");
		m_Texture = Apex::Texture2D::Create(m_TextureFilePath);

		auto& meshes = m_Model->GetMeshes();
		auto& meshBody1 = meshes.find("body_low1")->second;
		meshBody1->AddTexture(std::pair<std::string, Apex::Ref<Apex::Texture>>("TextureAlbedo",
				Apex::Texture2D::Create("assets/models/sci-fi-lasergun-gameready/textures/low_body_Reflection.png")));
		meshBody1->AddTexture(std::pair<std::string, Apex::Ref<Apex::Texture>>("TextureNormal",
				Apex::Texture2D::Create("assets/models/sci-fi-lasergun-gameready/textures/low_body_Normal.png")));
		meshBody1->AddTexture(std::pair<std::string, Apex::Ref<Apex::Texture>>("TextureMetallic",
				Apex::Texture2D::Create("assets/models/sci-fi-lasergun-gameready/textures/low_body_Metallic.png")));
		meshBody1->AddTexture(std::pair<std::string, Apex::Ref<Apex::Texture>>("TextureRoughness",
				Apex::Texture2D::Create("assets/models/sci-fi-lasergun-gameready/textures/low_body_Roughness.png")));

		auto& meshBody2 = meshes.find("body_low2")->second;
		meshBody2->AddTexture(std::pair<std::string, Apex::Ref<Apex::Texture>>("TextureAlbedo",
			Apex::Texture2D::Create("assets/models/sci-fi-lasergun-gameready/textures/low_metal_Diffuse.png")));
		meshBody2->AddTexture(std::pair<std::string, Apex::Ref<Apex::Texture>>("TextureNormal",
			Apex::Texture2D::Create("assets/models/sci-fi-lasergun-gameready/textures/low_metal_Normal.png")));
		meshBody2->AddTexture(std::pair<std::string, Apex::Ref<Apex::Texture>>("TextureMetallic",
			Apex::Texture2D::Create("assets/models/sci-fi-lasergun-gameready/textures/low_metal_Reflection.png")));
		meshBody2->AddTexture(std::pair<std::string, Apex::Ref<Apex::Texture>>("TextureRoughness",
			Apex::Texture2D::Create("assets/models/sci-fi-lasergun-gameready/textures/low_metal_Roughness.png")));		

		meshes.find("chick_low0")->second->Show() = false;
		meshes.find("mag_low3")->second->Show() = false;

		/*
		meshes.find("chick_low0")->second->AddTexture(std::pair<std::string, Apex::Ref<Apex::Texture>>("TextureDiffuse",
			Apex::Texture2D::Create("assets/models/sci-fi-lasergun-gameready/textures/low_mag_Diffuse.png")));
		meshes.find("mag_low3")->second->AddTexture(std::pair<std::string, Apex::Ref<Apex::Texture>>("TextureDiffuse",
			Apex::Texture2D::Create("assets/models/sci-fi-lasergun-gameready/textures/low_mag_Diffuse.png")));
		*/
		
		Apex::RenderCommands::SetCulling(true);
		Apex::RenderCommands::SetDepthTest(true);

		/*auto& shaderUniforms = m_Shader->GetActiveUniformLocations();
		for (auto[name, location] : shaderUniforms)
			APEX_TRACE("{0} : {1}", name, location);*/

		auto& shaderUniforms = m_Shader->GetActiveUniformData();
		for (auto[name, type, size] : shaderUniforms)
			APEX_LOG_TRACE("{0} : {1} [{2}]", name, type, size);

		glm::vec3 lightPositions[] = {
			{ 200.0f,  200.0f,  150.0f },
			{-200.0f, -200.0f, -150.0f }
		};

		glm::vec3 lightColors[] = {
			{ 3.3f, 3.1f, 2.4f },
			{ 3.3f, 3.1f, 2.4f }
		};

		m_Shader->SetUniFloat3v("u_LightPositions", lightPositions, 2);
		m_Shader->SetUniFloat3v("u_LightColors", lightColors, 2);

		m_LightCamera.SetPosition(lightPositions[0]);
		m_LightCamera.LookAt(glm::vec3(0.0f));

		m_DepthBuffer = Apex::DepthBuffer::Create();

		std::string depthShaderVertexSrc = R"(
			#version 450

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Model;

			void main()
			{
				gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
			}
		)";

		std::string depthShaderFragmentSrc = R"(
			#version 450

			void main() {}
		)";

		auto depthShader = Apex::Shader::Create("DepthShader", depthShaderVertexSrc, depthShaderFragmentSrc);
		Apex::AssetManager::GetShaderLibrary().Add(depthShader);

		Apex::AssetManager::GetShaderLibrary().Load("assets/shaders/Screen.glsl");

		m_ScreenVA = Apex::VertexArray::Create();
		Apex::Ref<Apex::VertexBuffer> screenVB;
		float screenVertices[] = {
			 0.6f,  1.0f,  0.0f, 1.0f,
			 0.6f,  0.6f,  0.0f, 0.0f,
			 1.0f,  0.6f,  1.0f, 0.0f,

			 0.6f,  1.0f,  0.0f, 1.0f,
			 1.0f,  0.6f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};
		screenVB = Apex::VertexBuffer::Create(screenVertices, sizeof(screenVertices));
		screenVB->SetLayout({
			{ Apex::ShaderDataType::Float2, "a_Position" },
			{ Apex::ShaderDataType::Float2, "a_TexCoord" }
			});
		m_ScreenVA->AddVertexBuffer(screenVB);
		m_ScreenVA->Unbind();


		m_SquareVA = Apex::VertexArray::Create();

		Apex::Ref<Apex::VertexBuffer> squareVB;
		float squareVertices[] = {
			-100.0f, 0.0f, -100.0f, 0.0f, 0.0f,
			-100.0f, 0.0f,  100.0f, 1.0f, 0.0f,
			 100.0f, 0.0f,  100.0f, 1.0f, 1.0f,
			 100.0f, 0.0f, -100.0f, 0.0f, 1.0f
		};
		squareVB = Apex::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
			{ Apex::ShaderDataType::Float3, "a_Position" },
			{ Apex::ShaderDataType::Float2, "a_TexCoord" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		Apex::Ref<Apex::IndexBuffer> squareIB;
		uint32_t squareIndices[] = {
			0, 1, 2,
			0, 2, 3
		};
		squareIB = Apex::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		m_SquareVA->AddIndexBuffer(squareIB);

		m_SquareVA->Unbind();

		auto textureShader = Apex::AssetManager::GetShaderLibrary().Load("assets/shaders/Texture.glsl");
		textureShader->Bind();
		textureShader->SetUniInt("u_Texture", 0);
	}
	
	virtual void OnAttach() override {}
	
	virtual void OnDetach() override {}
	
	virtual void OnUpdate() override
	{

		glm::vec3 cameraMovement(0.f);
		std::pair<float, float> mouseDiff;
		std::pair<float, float> mouseDiff1;
		auto[mouseX, mouseY] = Apex::Input::GetMousePos();
		if (!ImGui::IsAnyItemActive() && !ImGui::IsAnyWindowFocused()) {
			if (Apex::Input::IsKeyPressed(APEX_KEY_S))
				cameraMovement.z += m_CameraMoveSpeed * Apex::Timer::GetSeconds();
			if (Apex::Input::IsKeyPressed(APEX_KEY_W))
				cameraMovement.z -= m_CameraMoveSpeed * Apex::Timer::GetSeconds();
			if (Apex::Input::IsKeyPressed(APEX_KEY_A))
				cameraMovement.x -= m_CameraMoveSpeed * Apex::Timer::GetSeconds();
			if (Apex::Input::IsKeyPressed(APEX_KEY_D))
				cameraMovement.x += m_CameraMoveSpeed * Apex::Timer::GetSeconds();

			mouseDiff = {
				 Apex::Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_LEFT) ? (mouseX - m_MousePos.first) : 0.0f,
				 Apex::Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_LEFT) ? (mouseY - m_MousePos.second) : 0.0f
			};
			
			mouseDiff1 = {
				 Apex::Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_RIGHT) ? (mouseX - m_MousePos.first) : 0.0f,
				 Apex::Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_RIGHT) ? (mouseY - m_MousePos.second) : 0.0f
			};
		}
		m_MousePos = { mouseX, mouseY };
		if (m_MouseSelectMode)
			GetPicker();
		else
			m_Camera.Rotate(-mouseDiff.second * m_CameraRotateSpeed, mouseDiff.first * m_CameraRotateSpeed, 0.f);
		m_Camera.Move(cameraMovement);
		m_Shader->SetUniFloat3("u_CameraPosition", m_Camera.GetPosition());

		auto depthShader = Apex::AssetManager::GetShaderLibrary().GetShader("DepthShader");
		auto screenShader = Apex::AssetManager::GetShaderLibrary().GetShader("Screen");
		auto textureShader = Apex::AssetManager::GetShaderLibrary().GetShader("Texture");

		/*---------First Pass------------*/
		Apex::RenderCommands::SetViewport(0, 0, 2048, 2048);
		m_DepthBuffer->Bind();
		Apex::RenderCommands::Clear();
		Apex::Renderer::BeginScene(m_LightCamera);
		//Apex::Renderer::Submit(textureShader, m_SquareVA, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -12.0f, 0.0f)));
		Apex::Renderer::SubmitModel(depthShader, m_Model,
			glm::rotate(glm::mat4(1.0f), mouseDiff1.first * 0.02f, m_Camera.GetUp()) * glm::rotate(glm::mat4(1.0f), mouseDiff1.second * 0.02f, m_Camera.GetRight()));
		Apex::Renderer::EndScene();

		m_DepthBuffer->Unbind();
		Apex::RenderCommands::SetViewport(0, 0, 1280, 720);
		Apex::RenderCommands::SetClearColor({0.0f, 0.0f, 0.3f, 1.0f});
		Apex::RenderCommands::Clear();
		Apex::Renderer::BeginScene(m_Camera);

		m_Texture->Bind(0);
		m_DepthBuffer->GetDepthTexture()->Bind(1);
		textureShader->Bind();
		textureShader->SetUniMat4("u_LightSpace", m_LightCamera.GetViewProjectionMatrix());
		textureShader->SetUniInt("u_TextureLightDepth", 1);
		Apex::Renderer::Submit(textureShader, m_SquareVA, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -22.0f, 0.0f)));

		m_DepthBuffer->GetDepthTexture()->Bind(0);
		m_Shader->Bind();
		m_Shader->SetUniMat4("u_LightSpace", m_LightCamera.GetViewProjectionMatrix());
		m_Shader->SetUniInt("u_TextureLightDepth", 0);
		Apex::Renderer::SubmitModel(m_Shader, m_Model);

		screenShader->Bind();
		screenShader->SetUniInt("u_Texture", 0);
		Apex::Renderer::SubmitPostProcess(screenShader, m_ScreenVA);
		//Apex::Renderer::SubmitModel(m_Shader, m_Model,
		//	glm::rotate(glm::mat4(1.0f), mouseDiff1.first * 0.02f, m_Camera.GetUp()) * glm::rotate(glm::mat4(1.0f), mouseDiff1.second * 0.02f, m_Camera.GetRight()));

		Apex::Renderer::EndScene();
	}
	
	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Stats");
		std::stringstream ss;
		ss << "CameraPosition : " << m_Camera.GetPosition().x << "," << m_Camera.GetPosition().y << "," << m_Camera.GetPosition().z;
		ImGui::BulletText(ss.str().c_str());
		std::stringstream ss1;
		ss1 << "CameraRotation : " << m_Camera.GetRotation().x << "," << m_Camera.GetRotation().y << "," << m_Camera.GetRotation().z;
		ImGui::BulletText(ss1.str().c_str());
		ImGui::Checkbox("Select Mode", &m_MouseSelectMode);
		ImGui::DragFloat("Camera Move Speed", &m_CameraMoveSpeed, 1.0f, 2.0f, 100.0f);
		ImGui::InputText("Model File Path", m_ModelFilePath, 1024);
		if (ImGui::Button("Change Model")) {
			ChangeModel();
		}
		ImGui::InputText("Texture File Path", m_TextureFilePath, 1024);
		if (ImGui::Button("Change Texture")) {
			ChangeTexture();
		}
		ImGui::TextUnformatted(("Meshes Loaded :"));
		for (auto[name, mesh] : m_Model->GetMeshes()) {
			ImGui::Checkbox(name.c_str(), &(mesh->Show()));
		}
		ImGui::TextUnformatted("Textures Loaded :");
		for (auto[meshName, mesh] : m_Model->GetMeshes()) {
			for (auto[texName, texture] : mesh->GetTextures()) {
				ImGui::BulletText((meshName + " [" + texName + "] : " + texture->GetPath()).c_str());
			}
		}
		std::stringstream ss2;
		ss2 << "Selected Mesh : ";
		ImGui::TextUnformatted(ss2.str().c_str());
		ImGui::End();
	}

	void ChangeModel()
	{
		Apex::Ref<Apex::Model> newModel = Apex::Model::LoadModel(m_ModelFilePath);
		m_Model = newModel;
	}

	void ChangeTexture()
	{
		Apex::Ref<Apex::Texture2D> newTexture = Apex::Texture2D::Create(m_TextureFilePath);
		m_Texture = newTexture;
	}

	void GetPicker()
	{

	}

private:
	Apex::PerspectiveCamera m_Camera;
	Apex::Ref<Apex::Model> m_Model;
	Apex::Ref<Apex::Shader> m_Shader;
	Apex::Ref<Apex::Texture2D> m_Texture;

	Apex::Ref<Apex::DepthBuffer> m_DepthBuffer;
	Apex::OrthographicCamera m_LightCamera;

	Apex::Ref<Apex::VertexArray> m_ScreenVA;
	Apex::Ref<Apex::VertexArray> m_SquareVA;

	float m_CameraMoveSpeed = 30.0f;

	float m_CameraRotateSpeed = 0.6f;

	char m_ModelFilePath[1024] = "assets/models/sci-fi-lasergun-gameready/source/low.fbx";//"assets/models/sci-fi-rifle-m13-gaus/source/Gaus_M13_export.fbx";
	char m_TextureFilePath[1024] = "assets/textures/Checkerboard.png";

	std::pair<float, float> m_MousePos;

	bool m_MouseSelectMode = false;
};

class ParticleLayer : public Apex::Layer
{
public:
	ParticleLayer()
		: m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition({ 0.0f, 0.0f, 0.0f }), m_ParticleSystem(5, 10000)
	{
		m_Camera.SetPosition({ 0.0f, 0.0f, 0.0f });
		m_Camera.SetRotation(0.0f);

		// Position and Lifetime
		m_ParticleProps.position = { 0.0f, 0.0f };
		m_ParticleProps.lifetime = 1.0f;

		// Movement
		m_ParticleProps.velocity = { 0.0f, 1.0f };
		m_ParticleProps.velocityVariation = { 0.45f, 0.36f };
		m_ParticleProps.rotationSpeed = 2.0f;
		m_ParticleProps.rotationSpeedVariation = 1.0f;

		// Size
		m_ParticleProps.sizeBegin = 0.7f;
		m_ParticleProps.sizeEnd = 0.0085f;
		m_ParticleProps.sizeVariation = 0.3f;
		
		// Color
		m_ParticleProps.colorBegin = { 0.85f, 0.64f, 0.0f, 1.0f };
		m_ParticleProps.colorEnd = { 0.1f, 0.1f, 0.1f, 1.0f };

		m_ParticleTexture = Apex::Texture2D_HDR::Create("assets/textures/smoke.png");
		m_ParticleTexture->SetRows(2);
		m_ParticleProps.textureNumRows = m_ParticleTexture->GetRows();
		//Apex::RenderCommands::SetBlendMode(Apex::RendererAPI::BlendingMode::SRC_ALPHA, Apex::RendererAPI::BlendingMode::ONE_MINUS_DST_ALPHA);

		m_ParticleSystem.Emit(m_ParticleProps);
	}

	void OnAttach() override {}
	void OnDetach() override {}
	
	void OnUpdate() override
	{
		auto[mouseX, mouseY] = Apex::Input::GetMousePos();
		if (!ImGui::IsAnyItemActive() && !ImGui::IsAnyWindowFocused()) {
			if (Apex::Input::IsKeyPressed(APEX_KEY_S))
				m_ParticleProps.position.y -= m_ParticleSystemMoveSpeed * Apex::Timer::GetSeconds();
			if (Apex::Input::IsKeyPressed(APEX_KEY_W))
				m_ParticleProps.position.y += m_ParticleSystemMoveSpeed * Apex::Timer::GetSeconds();
			if (Apex::Input::IsKeyPressed(APEX_KEY_A))
				m_ParticleProps.position.x -= m_ParticleSystemMoveSpeed * Apex::Timer::GetSeconds();
			if (Apex::Input::IsKeyPressed(APEX_KEY_D))
				m_ParticleProps.position.x += m_ParticleSystemMoveSpeed * Apex::Timer::GetSeconds();

			std::pair<float, float> mouseDiff = {
				 Apex::Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_LEFT) ? (mouseX - m_MousePos.first) : 0.0f,
				 Apex::Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_LEFT) ? (mouseY - m_MousePos.second) : 0.0f
			};
			//m_CameraRotation += Apex::Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_RIGHT) ? ((mouseY - m_MousePos.second) * m_CameraRotateSpeed * Apex::Timer::GetSeconds()) : 0.0f;
			m_CameraPosition.x -= mouseDiff.first * m_CameraMoveSpeed;
			m_CameraPosition.y += mouseDiff.second * m_CameraMoveSpeed;
		}
		m_MousePos = { mouseX, mouseY };
		m_Camera.SetPosition(m_CameraPosition);

		/*m_ParticleProps.textureIndex = Apex::Random::Int() % m_ParticleTexture->GetMaxIndex();
		for (int i = 0; i < m_ParticleGenerationSpeed; i++) {
			if(Apex::Random::Float() < 0.002)
				m_ParticleSystem.Emit(m_ParticleProps);
		}*/

		Apex::RenderCommands::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		Apex::RenderCommands::Clear();
		Apex::Renderer::BeginScene(m_Camera);
		m_ParticleSystem.OnUpdate();
		m_ParticleTexture->Bind(0);
		m_ParticleSystem.OnRender();
		Apex::Renderer::EndScene();
	}
	
	void OnImGuiRender() override
	{
		ImGui::ShowMetricsWindow();
		ImGui::Begin("Particle System 2D");
		ImGui::DragFloat2("Position", &m_ParticleProps.position.x, 0.05f, -1.0f, 1.0f);
		ImGui::DragFloat("Lifetime", &m_ParticleProps.lifetime, 0.01f, 0.0f, 5.0f);
		ImGui::SliderInt("Particle Generation Speed", (int*)&m_ParticleGenerationSpeed, 1, 10);
		ImGui::DragFloat2("Velocity", &m_ParticleProps.velocity.x, 0.005f);
		ImGui::DragFloat2("Velocity Variation", &m_ParticleProps.velocityVariation.x, 0.005f);
		ImGui::DragFloat("Start Size", &m_ParticleProps.sizeBegin, 0.005f, 0.0f, 1.0f);
		ImGui::DragFloat("End Size", &m_ParticleProps.sizeEnd, 0.005f, 0.0f, 1.0f);
		ImGui::ColorPicker4("Start Color", &m_ParticleProps.colorBegin.x);
		ImGui::ColorPicker4("End Color", &m_ParticleProps.colorEnd.x);
		ImGui::Image((void*)(intptr_t)m_ParticleTexture->GetID(), ImVec2(256, 256));
		ImGui::End();
	}

	void OnEvent(Apex::Event& event) override
	{
		Apex::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<Apex::MouseScrolledEvent>(APEX_BIND_EVENT_FN(ParticleLayer::OnMouseScrolled));
	}

	bool OnMouseScrolled(Apex::MouseScrolledEvent& event)
	{
		if(event.GetOffsetY() < 0)
			m_ParticleProps.textureIndex = m_ParticleTexture->GetMaxIndex() - ((m_ParticleTexture->GetMaxIndex() - m_ParticleProps.textureIndex--) % m_ParticleTexture->GetMaxIndex()) - 1;
		else
			m_ParticleProps.textureIndex = m_ParticleTexture->GetMaxIndex() - ((m_ParticleTexture->GetMaxIndex() - m_ParticleProps.textureIndex++) % m_ParticleTexture->GetMaxIndex()) - 1;
		m_ParticleSystem.Emit(m_ParticleProps);
		return false;
	}

private:
	Apex::ParticleSystem2D m_ParticleSystem;
	Apex::ParticleProps2D m_ParticleProps;
	Apex::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraRotation = 0.0f;
	float m_CameraMoveSpeed = 0.0025f;
	float m_CameraRotateSpeed = 30.0f;

	float m_ParticleSystemMoveSpeed = 0.75f;
	uint32_t m_ParticleGenerationSpeed = 5;

	Apex::Ref<Apex::Texture2D_HDR> m_ParticleTexture;

	std::pair<float, float> m_MousePos = { 0.0f, 0.0f };
};

class Sandbox : public Apex::Application
{
public:
	Sandbox()
	{
		//PushLayer(new SandboxLayer());
		//PushLayer(new ModelLayer());
		//PushLayer(new ParticleLayer());
		PushLayer(new DXBall::GameLayer());
	}

	~Sandbox()
	{

	}

};

Apex::Application* Apex::CreateApplication()
{
	return new Sandbox();
}