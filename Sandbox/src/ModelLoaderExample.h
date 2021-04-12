#pragma once

class ModelLayer : public Apex::Layer
{
public:
	ModelLayer()
		: Layer("Model"), m_Camera(63.5f, 16.f / 9.f), m_LightCamera(-150.f, 150.f, -150.f, 150.f, 100.0f, 410.0f)
	{
		
	}
	
	virtual void OnAttach() override
	{
		APEX_PROFILE_FUNC();

		m_Camera.SetPosition({ 0.f, 0.f, 10.f });

		m_Model = Apex::Model::LoadModel(m_ModelFilePath);
		m_Shader = Apex::Shader::Create("assets/shaders/PBR.glsl");
		m_Shader->Bind();
		m_Shader->SetUniInt1("u_TextureAlbedo", 0);
		m_Shader->SetUniInt1("u_TextureNormal", 1);
		m_Shader->SetUniInt1("u_TextureMetallic", 2);
		m_Shader->SetUniInt1("u_TextureRoughness", 3);
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

		//auto& shaderUniforms = m_Shader->GetActiveUniformData();
		//for (auto [name, type, size] : shaderUniforms)
		//	APEX_LOG_TRACE("{0} : {1} [{2}]", name, type, size);

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
		textureShader->SetUniInt1("u_Texture", 0);

		m_SoundEngine = irrklang::createIrrKlangDevice();
		m_SoundEngine->setListenerPosition(irrklang::vec3df(m_Camera.GetPosition().x, m_Camera.GetPosition().y, m_Camera.GetPosition().z), 
			irrklang::vec3df(m_Camera.GetFront().x, m_Camera.GetFront().y, m_Camera.GetFront().z));
		m_Sound = m_SoundEngine->play3D("assets/audio/breakout.mp3", irrklang::vec3df(0.0f, 0.0f, 0.0f), true, false, true);
	}
	
	virtual void OnDetach() override {}
	
	virtual void OnUpdate() override
	{
		APEX_PROFILE_FUNC();

		glm::vec3 cameraMovement(0.f);
		std::pair<float, float> mouseDiff;
		std::pair<float, float> mouseDiff1;
		{
			APEX_PROFILE_SCOPE("Camera-Movement");
			auto [mouseX, mouseY] = Apex::Input::GetMousePos();
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

				if (glm::length(cameraMovement) > 0.0)
					APEX_PROFILE_EVENT("Camera Moved");

				if (glm::length(glm::vec2(mouseDiff.first, mouseDiff.second)) > 0.0)
					APEX_PROFILE_EVENT("Camera Rotated");

				if (glm::length(glm::vec2(mouseDiff1.first, mouseDiff1.second)) > 0.0)
					APEX_PROFILE_EVENT("Model Rotated");

			}
			m_MousePos = { mouseX, mouseY };
			if (m_MouseSelectMode)
				GetPicker();
			else
				m_Camera.Rotate(-mouseDiff.second * m_CameraRotateSpeed, mouseDiff.first * m_CameraRotateSpeed, 0.f);
			m_Camera.Move(cameraMovement);
			m_Shader->SetUniFloat3("u_CameraPosition", m_Camera.GetPosition());

			m_SoundEngine->setListenerPosition(irrklang::vec3df(m_Camera.GetPosition().x, m_Camera.GetPosition().y, -m_Camera.GetPosition().z),
				irrklang::vec3df(m_Camera.GetFront().x, m_Camera.GetFront().y, -m_Camera.GetFront().z));
		}

		auto depthShader = Apex::AssetManager::GetShaderLibrary().GetShader("DepthShader");
		auto screenShader = Apex::AssetManager::GetShaderLibrary().GetShader("Screen");
		auto textureShader = Apex::AssetManager::GetShaderLibrary().GetShader("Texture");

		/*---------Shadow Pass------------*/
		{
			APEX_PROFILE_SCOPE("Shadow-Pass");
			Apex::RenderCommands::SetViewport(0, 0, 2048, 2048);
			m_DepthBuffer->Bind();
			Apex::RenderCommands::Clear();
			Apex::Renderer::BeginScene(m_LightCamera);
			//Apex::Renderer::Submit(textureShader, m_SquareVA, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -12.0f, 0.0f)));
			Apex::Renderer::SubmitModel(depthShader, m_Model,
				glm::rotate(glm::mat4(1.0f), mouseDiff1.first * 0.02f, m_Camera.GetUp()) * glm::rotate(glm::mat4(1.0f), mouseDiff1.second * 0.02f, m_Camera.GetRight()));
			Apex::Renderer::EndScene();
		}

		/*---------Main Pass------------*/
		{
			APEX_PROFILE_SCOPE("Main-Pass");
			m_DepthBuffer->Unbind();
			Apex::RenderCommands::SetViewport(0, 0, 1280, 720);
			Apex::RenderCommands::SetClearColor({ 0.0f, 0.0f, 0.3f, 1.0f });
			Apex::RenderCommands::Clear();
			Apex::Renderer::BeginScene(m_Camera);
			
			{
				APEX_PROFILE_SCOPE("Submit-Quad");
				m_Texture->Bind(0);
				m_DepthBuffer->GetDepthTexture()->Bind(1);
				textureShader->Bind();
				textureShader->SetUniMat4("u_LightSpace", m_LightCamera.GetViewProjectionMatrix());
				textureShader->SetUniInt1("u_TextureLightDepth", 1);
				Apex::Renderer::Submit(textureShader, m_SquareVA, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -22.0f, 0.0f)));
			}
			{
				APEX_PROFILE_SCOPE("Submit-Model");
				m_DepthBuffer->GetDepthTexture()->Bind(0);
				m_Shader->Bind();
				m_Shader->SetUniMat4("u_LightSpace", m_LightCamera.GetViewProjectionMatrix());
				m_Shader->SetUniInt1("u_TextureLightDepth", 0);
				Apex::Renderer::SubmitModel(m_Shader, m_Model);
			}
			{
				APEX_PROFILE_SCOPE("Post-Process");
				screenShader->Bind();
				screenShader->SetUniInt1("u_Texture", 0);
				Apex::Renderer::SubmitPostProcess(screenShader, m_ScreenVA);
				//Apex::Renderer::SubmitModel(m_Shader, m_Model,
				//	glm::rotate(glm::mat4(1.0f), mouseDiff1.first * 0.02f, m_Camera.GetUp()) * glm::rotate(glm::mat4(1.0f), mouseDiff1.second * 0.02f, m_Camera.GetRight()));
			}
			Apex::Renderer::EndScene();
		}
	}
	
	virtual void OnImGuiRender() override
	{
		ImGui::ShowMetricsWindow();

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

	irrklang::ISoundEngine* m_SoundEngine;
	irrklang::ISound* m_Sound;

	float m_CameraMoveSpeed = 30.0f;

	float m_CameraRotateSpeed = 0.6f;

	char m_ModelFilePath[1024] = "assets/models/sci-fi-lasergun-gameready/source/low.fbx";//"assets/models/sci-fi-rifle-m13-gaus/source/Gaus_M13_export.fbx";
	char m_TextureFilePath[1024] = "assets/textures/Checkerboard.png";

	std::pair<float, float> m_MousePos;

	bool m_MouseSelectMode = false;
}; 
