#pragma once

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
		m_ParticleProps.lifetime = 10.0f;

		// Movement
		m_ParticleProps.velocity = { 0.0f, 0.1f };
		m_ParticleProps.velocityVariation = { 0.140f, 0.27f };
		m_ParticleProps.rotationSpeed = 2.0f;
		m_ParticleProps.rotationSpeedVariation = 1.0f;
		m_ParticleProps.acceleration = { 0.0f, -1.0f };

		// Size
		m_ParticleProps.sizeBegin = { 0.05f, 0.05f };
		m_ParticleProps.sizeEnd = { 0.0085f, 0.0085f };
		m_ParticleProps.sizeVariation = { 0.03f, 0.03f };
		
		// Color
		m_ParticleProps.colorBegin = { 0.85f, 0.64f, 0.0f, 1.0f };
		m_ParticleProps.colorEnd = { 0.1f, 0.1f, 0.1f, 1.0f };

		m_ParticleProps.useTexture = false;

		//m_ParticleSystem.Emit(m_ParticleProps);
	}

	void OnAttach() override
	{
		//m_ParticleTexture = Apex::Texture2D_HDR::Create("assets/textures/fern.png");
		//m_ParticleTexture->SetRows(2);
		//m_ParticleProps.textureNumRows = m_ParticleTexture->GetRows();
		//Apex::RenderCommands::SetBlendMode(Apex::RendererAPI::BlendingMode::SRC_ALPHA, Apex::RendererAPI::BlendingMode::ONE_MINUS_DST_ALPHA);
	}
	
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
			//APEX_LOG_DEBUG("mouseXY: {0}, {1}", mouseX, mouseY);
			//APEX_LOG_DEBUG("mouseDiff: {0}, {1}", mouseDiff.first, mouseDiff.second);
			m_CameraPosition.x -= mouseDiff.first * m_CameraMoveSpeed;
			m_CameraPosition.y += mouseDiff.second * m_CameraMoveSpeed;
		}
		m_MousePos = { mouseX, mouseY };

		m_MousePosInFrame = m_MousePos;
		m_MousePosInFrame.first -= 640.0f;
		m_MousePosInFrame.first /= 640.0f;
		m_MousePosInFrame.first *= 1.6f;
		m_MousePosInFrame.second -= 360.0f;
		m_MousePosInFrame.second /= -360.0f;
		m_MousePosInFrame.second *= 0.9f;

		m_Camera.SetPosition(m_CameraPosition);

		//m_ParticleProps.textureIndex = Apex::Random::Int() % m_ParticleTexture->GetMaxIndex();
		for (int i = 0; i < m_ParticleGenerationSpeed; i++) {
			auto probability = m_ParticleGenerationProbability;
			if(Apex::Random::Float() < /*0.02f*/ probability) {
				m_ParticleProps.rotation = Apex::Random::Float() * 2.0f * glm::pi<float>();
				m_ParticleSystem.Emit(m_ParticleProps);
			}
		}
		m_ParticleSystem.OnUpdate();

		Apex::RenderCommands::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		Apex::RenderCommands::Clear();
		Apex::Renderer::BeginScene(m_Camera);
		//m_ParticleTexture->Bind(0);
		m_ParticleSystem.OnRender();
		Apex::Renderer::EndScene();
	}
	
	void OnImGuiRender() override
	{
		ImGui::ShowMetricsWindow();
		
		ImGui::Begin("Particle System 2D");
		ImGui::DragFloat2("Position", &m_ParticleProps.position.x, 0.05f, -1.0f, 1.0f);
		ImGui::DragFloat("Lifetime", &m_ParticleProps.lifetime, 0.1f, 0.0f, 50.0f);
		ImGui::SliderInt("Particle Generation Speed", (int*)&m_ParticleGenerationSpeed, 1, 10);
		ImGui::DragFloat("Particle Probability", &m_ParticleGenerationProbability, 0.005f, 0.0f, 1.0f);
		ImGui::DragFloat2("Velocity", &m_ParticleProps.velocity.x, 0.005f);
		ImGui::DragFloat2("Velocity Variation", &m_ParticleProps.velocityVariation.x, 0.005f);
		ImGui::DragFloat2("Acceleration", &m_ParticleProps.acceleration.x, 0.005f);
		ImGui::DragFloat2("Start Size", &m_ParticleProps.sizeBegin.x, 0.005f, 0.0f, 1.0f);
		ImGui::DragFloat2("End Size", &m_ParticleProps.sizeEnd.x, 0.005f, 0.0f, 1.0f);
		ImGui::ColorPicker4("Start Color", &m_ParticleProps.colorBegin.x);
		ImGui::ColorPicker4("End Color", &m_ParticleProps.colorEnd.x);
		//ImGui::Image((void*)(intptr_t)m_ParticleTexture->GetID(), ImVec2(256, 256));
		ImGui::End();
		
		ImGui::Begin("Camera Control");
		ImGui::DragFloat2("Position", &m_CameraPosition.x, 0.05f, -1.0f, 1.0f);
		ImGui::End();
		
		/*static bool unselected = true;
		ImGui::Begin("Client-Server");
		if (unselected) {
			auto fun = [](const Apex::Ref<Apex::TCPClientSocket>&, Apex::CommunicationPacket&) { return false; };

			if (ImGui::Button("Client")) {
				Apex::NetworkManager::StartClientWorkerThread(std::make_shared<Apex::TCPClientSocket>("localhost", "27015"), fun, std::chrono::microseconds(25));
				unselected = false;
			}
			if (ImGui::Button("Server")) {
				Apex::NetworkManager::StartServerWorkerThread(fun, std::chrono::microseconds(25));
				unselected = false;
			}
		}
		ImGui::End();*/
	}

	void OnEvent(Apex::Event& event) override
	{
		Apex::EventDispatcher dispatcher(event);
		//dispatcher.Dispatch<Apex::MouseScrolledEvent>(APEX_BIND_EVENT_FN(ParticleLayer::OnMouseScrolled));
		dispatcher.Dispatch<Apex::KeyPressedEvent>([this] (auto& event) { 
// 			m_ParticleProps.position = { m_MousePos.first, m_MousePos.second };
			//m_ParticleProps.rotation = Apex::Random::Float() * 2.0f * glm::pi<float>();
			if (event.GetKeyCode() == APEX_KEY_SPACE) {
				m_ParticleProps.rotation = Apex::Random::Float() * 2.0f * glm::pi<float>();
				m_ParticleSystem.Emit(m_ParticleProps);
			}
			return true;
		});
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
	uint32_t m_ParticleGenerationSpeed = 2;
	float m_ParticleGenerationProbability = 0.02f;

	Apex::Ref<Apex::Texture2D_HDR> m_ParticleTexture;

	std::pair<float, float> m_MousePos = { 0.0f, 0.0f };
	std::pair<float, float> m_MousePosInFrame = { 0.0f, 0.0f };
};
 
