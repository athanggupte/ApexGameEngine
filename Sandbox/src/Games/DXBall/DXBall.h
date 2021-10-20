#pragma once
#include <Apex.h>
#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/compatibility.hpp>

#include "Apex/Physics/PhysicsTimer/PhysicsTimer.h"
#include "Apex/Networking/NetworkManager.h"

#include "GameObjects/Ball.h"
#include "GameObjects/Paddle.h"
#include "GameObjects/Brick.h"

#include "Gameplay/Lobby.h"

namespace DXBall {

	class GameLayer : public Apex::Layer
	{
	public:
		GameLayer()
			: m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition({ 0.0f, 0.0f, 0.0f }), m_ParticleSystem(5, 10000),
			m_Position({ 0.0f,0.0f })//, m_Velocity({ 1.5f, 1.0f }), m_Acceleration({ 0.0f, -5.0f })
		{
			//m_Ball_1 = std::make_unique<Ball>(m_Position, 0.05f);
			//m_Ball_2 = std::make_unique<Ball>(glm::vec2(0.78f, -0.43f), 0.20f, glm::vec2(-2.0f, 1.0f));

			m_Balls.resize(m_NumberOfBalls);
			m_Bricks.resize(5);

			for (int i = 0; i < m_Balls.size(); i++) {
				m_Balls[i] = std::make_unique<Ball>(
					glm::vec2(2.0f * Apex::Random::Float() - 1.0f, 2.0f * Apex::Random::Float() - 1.0f),
					0.05f,
					glm::vec2(2.0f * Apex::Random::Float() - 1.0f, 2.0f * Apex::Random::Float() - 1.0f)
				);
			}

			for (int i = 0; i < m_Bricks.size(); i++) {
				m_Bricks[i] = std::make_unique<Brick>(glm::vec2(-0.90f + i * 0.40f, 0.80f), glm::vec2(-0.70f + i * 0.40f, 0.70f));
			}

			Apex::PhysicsTimer::Init();
			m_Camera.SetPosition({ 0.0f, 0.0f, 0.0f });
			m_Camera.SetRotation(0.0f);
			
			// Position and Lifetime
			m_ParticleProps.position = { 0.0f, 0.0f };
			m_ParticleProps.lifetime = 1.0f;
			m_ParticleProps.acceleration = { 0.0f, -3.5f };

			// Movement
			m_ParticleProps.velocity = { 0.0f, 1.0f };
			m_ParticleProps.velocityVariation = { 0.45f, 0.36f };
			m_ParticleProps.rotationSpeed = 0.0f;
			m_ParticleProps.rotationSpeedVariation = 0.0f;

			// Size
			m_ParticleProps.sizeBegin = { 0.0023f, 0.0023f };
			m_ParticleProps.sizeEnd = { 0.0085f, 0.0085 };
			m_ParticleProps.sizeVariation = { 0.025f, 0.025f };

			// Color
			m_ParticleProps.colorBegin = { 5.85f, 2.64f, 0.0f, 0.8f };
			m_ParticleProps.colorEnd = { 0.1f, 0.1f, 0.1f, 0.0f };

			// Texture
			m_ParticleProps.useTexture = false;
			m_ParticleTexture = Apex::Texture2D_HDR::Create("assets/textures/tennis-ball.png");
			m_ParticleTexture->SetRows(1);
			m_ParticleProps.textureNumRows = 0;// m_ParticleTexture->GetRows();
			Apex::RenderCommands::SetBlendMode(Apex::RendererAPI::BlendingMode::ONE, Apex::RendererAPI::BlendingMode::ONE_MINUS_SRC_ALPHA);

			m_ParticleSystem.Emit(m_ParticleProps);
			
		}

		void OnAttach() override
		{
			Apex::PhysicsTimer::SetPhysicsFunction([this] (std::chrono::nanoseconds diffTime) { PerformPhysicsCalc(diffTime); }, 8);
			Apex::PhysicsTimer::SetPhysicsFunction([this] (std::chrono::nanoseconds diffTime) { m_ParticleSystem.OnUpdate(); }, 8);
		}

		void OnDetach() override {}

		void OnUpdate() override
		{
			m_MousePos = Apex::Input::GetMousePos();
			m_MousePosInFrame = m_MousePos;
			m_MousePos.first -= 640.0f;
			m_MousePos.first /= 640.0f;
			m_MousePos.first *= 1.6f;
			m_MousePos.second -= 360.0f;
			m_MousePos.second /= -360.0f;
			m_MousePos.second *= 0.9f;
			//m_ParticleProps.textureIndex = Apex::Random::Int() % m_ParticleTexture->GetMaxIndex();

			//m_ParticleProps.velocity = m_Velocity;
			//m_ParticleProps.position = m_Position;


			//for (int i = 0; i < m_ParticleGenerationSpeed; i++) {
				//if(Apex::Random::Float() <= 1.0)
			//		m_ParticleSystem.Emit(m_ParticleProps);
			//}

			Apex::RenderCommands::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
			Apex::RenderCommands::Clear();
			Apex::Renderer::BeginScene(m_Camera);
			//m_ParticleTexture->Bind(0);
			m_ParticleSystem.OnRender();
			m_ParticleTexture->Bind(0);
			//m_Ball_1->Render();
			//m_Ball_2->Render();
			for(auto& brick : m_Bricks)
				brick->Render();
			for (auto& ball : m_Balls) {
				ball->Render();
			}
			Apex::Renderer::EndScene();
		}

		void OnImGuiRender() override
		{
			ImGui::ShowMetricsWindow();
			
			/*ImGui::Begin("Particle System 2D");
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
			ImGui::End();*/

			ImGui::Begin("DX Ball");
			//ImGui::DragInt("Number of Balls", &m_NumberOfBalls, 1.0f, 1, 17);
			//ImGui::DragFloat2("Brick", &m_Brick->m_Position.x, 0.005f, -1.0f, 1.0f);
			ImGui::DragFloat2("Position", &m_Position.x, 0.05f, -1.0f, 1.0f);
			ImGui::DragFloat2("Mouse", &m_MousePos.first);
			int i = 0;
			for (auto& ball : m_Balls) {
				std::stringstream ss;
				ss << "Ball #" << i << " " << ball->GetPosition().x << " , " << ball->GetPosition().y;
				ImGui::TextColored({}, ss.str().c_str());
			}
			ImGui::End();

			if (m_MouseOver) {
				ImGui::SetNextWindowPos({ m_MousePosInFrame.first, m_MousePosInFrame.second });
				ImGui::Begin("Mouse Selection");
				ImGui::TextUnformatted(m_MouseOverObject.c_str());
				ImGui::End();
			}
		}

		void OnEvent(Apex::Event& event) override
		{
			Apex::EventDispatcher dispatcher(event);
			dispatcher.Dispatch<Apex::MouseScrolledEvent>(APEX_BIND_EVENT_FN(GameLayer::OnMouseScrolled));
		}

		bool OnMouseScrolled(Apex::MouseScrolledEvent& event)
		{
			/*if (event.GetOffsetY() < 0)
				m_ParticleProps.textureIndex = m_ParticleTexture->GetMaxIndex() - ((m_ParticleTexture->GetMaxIndex() - m_ParticleProps.textureIndex--) % m_ParticleTexture->GetMaxIndex()) - 1;
			else
				m_ParticleProps.textureIndex = m_ParticleTexture->GetMaxIndex() - ((m_ParticleTexture->GetMaxIndex() - m_ParticleProps.textureIndex++) % m_ParticleTexture->GetMaxIndex()) - 1;
			m_ParticleSystem.Emit(m_ParticleProps);*/
			return false;
		}

		inline void PerformPhysicsCalc(std::chrono::nanoseconds diffTime)
		{
			//APEX_LOG_DEBUG("Doing Physics Calculations ...");
			//m_Ball_1->SetPosition(m_Position);
			//m_Ball_1->Update();
			//m_Ball_2->Update();
			//m_Brick->Update();

			for (auto& ball : m_Balls)
				ball->Update();
			//m_ParticleProps.position = m_Ball_1->GetPosition();
			//m_ParticleProps.velocity = -m_Ball_1->GetVelocity() * 0.1f;
			m_MouseOver = false;
			m_MouseOverObject = "";
			glm::vec2 mousePos = { m_MousePos.first, m_MousePos.second };
			//mousePos 

			//auto [topLeft, bottomRight] = std::dynamic_pointer_cast<Apex::AABBCollider>(m_Brick->GetCollider())->GetCorners();
			//glm::vec2 collisionDir = { 0.0f, 0.0f };
			// Collisions
			// Between balls
			std::vector<bool> brickCollision;
			brickCollision.resize(m_Bricks.size());

			for (int i = 0; i < m_Balls.size()-1; i++) {
				for (int j = i + 1; j < m_Balls.size(); j++) {
					auto& ball_1 = m_Balls[i];
					auto& ball_2 = m_Balls[j];
					
					if (ball_1->GetCollider()->IsColliding(ball_2->GetCollider())) {
						auto v1 = ball_1->GetVelocity();
						auto v2 = ball_2->GetVelocity();

						auto pos1 = ball_1->GetPosition();
						auto pos2 = ball_2->GetPosition();

						//APEX_LOG_DEBUG("pos1 : {0},{1} \t||\t pos2 : {2},{3}", pos1.x, pos1.y, pos2.x, pos2.y);

						glm::vec2 un = glm::normalize(pos1 - pos2);//collision.normal;
						glm::vec2 ut = glm::vec2(-un.y, un.x);

						//APEX_LOG_DEBUG("un : {0},{1} \t||\t ut : {2},{3}", un.x, un.y, ut.x, ut.y);

						float v1n = glm::dot(un, v1);
						float v1t = glm::dot(ut, v1);

						float v2n = glm::dot(un, v2);
						float v2t = glm::dot(ut, v2);

						//APEX_LOG_INFO("v1n : {0} || v1t : {1} \t||\t v2n : {2} || v2t : {3}", v1n, v1t, v2n, v2t);

						auto v1_ = glm::vec2(v2n * un + v1t * ut);
						auto v2_ = glm::vec2(v1n * un + v2t * ut);

						//APEX_LOG_TRACE("v1_ : {0},{1} \t||\t v2_ : {2},{3}", v1_.x, v1_.y, v2_.x, v2_.y);

						ball_1->SetVelocity(v1_);
						ball_2->SetVelocity(v2_);

						float r1 = std::dynamic_pointer_cast<Apex::CircleCollider>(ball_1->GetCollider())->GetRadius();
						float r2 = std::dynamic_pointer_cast<Apex::CircleCollider>(ball_2->GetCollider())->GetRadius();

						m_ParticleProps.position = glm::lerp(pos1, pos2, r1 / r2);
						for (int i = 0; i < 8; i++) {
							float angle = Apex::Random::Float() * 2.0f * glm::pi<float>();
							m_ParticleProps.rotation = angle;
							m_ParticleProps.velocity = { glm::cos(angle), glm::sin(angle) };
							m_ParticleProps.sizeBegin = glm::vec2(0.0085f, 0.00003f);
							m_ParticleSystem.Emit(m_ParticleProps);
						}
					}
				}
				// Between balls and bricks
				int brickIndex = 0;
				for (auto& brick : m_Bricks) {
					if (m_Balls[i]->GetCollider()->IsColliding(brick->GetCollider())) {
						brickCollision[brickIndex] = true;
						m_Balls[i]->m_UseColor = true;
						m_Balls[i]->u_Color = { 1.0f, 0.0f, 0.0f, 1.0f };

						auto v = m_Balls[i]->GetVelocity();

						glm::vec2 un = Apex::Collision::GetNormal();
						glm::vec2 ut = glm::vec2(-un.y, un.x);

						float vn = glm::dot(un, v);
						float vt = glm::dot(ut, v);

						auto v_ = glm::vec2(-vn * un + vt * ut);

						m_Balls[i]->SetVelocity(v_);
					}
					brickIndex++;
				}
				/*else if (m_Balls[i]->GetCollider()->IsUnderMouse(mousePos)) {
					m_Balls[i]->m_UseColor = true;
					m_Balls[i]->u_Color = { 0.0f, 1.0f, 0.0f, 1.0f };
					m_MouseOver = true;
					std::stringstream ss;
					ss << "Ball #" << (i + 1);
					m_MouseOverObject = ss.str();
				}
				else
					m_Balls[i]->m_UseColor = false;*/

			}
			int brickIndex = 0;
			for (auto& brick : m_Bricks) {
				if (m_Balls.back()->GetCollider()->IsColliding(brick->GetCollider())) {
					brickCollision[brickIndex] = true;
					m_Balls.back()->m_UseColor = true;
					m_Balls.back()->u_Color = { 1.0f, 0.0f, 0.0f, 1.0f };

					auto v = m_Balls.back()->GetVelocity();

					glm::vec2 un = Apex::Collision::GetNormal();
					glm::vec2 ut = glm::vec2(-un.y, un.x);

					float vn = glm::dot(un, v);
					float vt = glm::dot(ut, v);

					auto v_ = glm::vec2(-vn * un + vt * ut);

					m_Balls.back()->SetVelocity(v_);
				}
				brickIndex++;
			}
			/*else if (m_Balls.back()->GetCollider()->IsUnderMouse(mousePos)) {
				m_Balls.back()->m_UseColor = true;
				m_Balls.back()->u_Color = { 0.0f, 1.0f, 0.0f, 1.0f };
				m_MouseOver = true;
				std::stringstream ss;
				ss << "Ball #" << m_Balls.size();
				m_MouseOverObject = ss.str();
			}
			else 
				m_Balls.back()->m_UseColor = false;*/

			// Check Brick with mouse
			brickIndex = 0;
			for (auto& brick : m_Bricks) {
				if (brick->GetCollider()->IsUnderMouse(mousePos)) {
					brick->m_Color = { 0.0f, 1.0f, 0.0f, 1.0f };
					m_MouseOver = true;
					m_MouseOverObject = "Brick #" + brickIndex;
				}
				else if (brickCollision[brickIndex])
					brick->m_Color = { 1.0f, 0.0f, 0.0f, 1.0f };
				else
					brick->m_Color = brick->c_Color;

				brickIndex++;
			}
			
		}

	private:
		Apex::Unique<Ball> m_Ball_1;
		//Apex::Unique<Ball> m_Ball_2;

		std::vector<Apex::Unique<Brick>> m_Bricks;
		Apex::Unique<Paddle> m_Paddle;
		std::vector<Apex::Unique<Ball>> m_Balls;
		int m_NumberOfBalls = 12;

		Apex::ParticleSystem2D m_ParticleSystem;
		Apex::ParticleProps2D m_ParticleProps;
		Apex::OrthographicCamera m_Camera;
		glm::vec3 m_CameraPosition;
		float m_CameraRotation = 0.0f;
		float m_CameraMoveSpeed = 0.0025f;
		float m_CameraRotateSpeed = 30.0f;

		float m_ParticleSystemMoveSpeed = 0.75f;
		uint32_t m_ParticleGenerationSpeed = 5;

		glm::vec2 m_Position;
		//std::atomic<glm::vec2> m_Velocity;
		//std::atomic<glm::vec2> m_Acceleration;

		Apex::Ref<Apex::Texture2D_HDR> m_ParticleTexture;

		std::pair<float, float> m_MousePos = { 0.0f, 0.0f };
		std::pair<float, float> m_MousePosInFrame = { 0.0f, 0.0f };

		bool m_MouseOver = false;
		std::string m_MouseOverObject = "";
	};

	class TestLayer : public Apex::Layer
	{
	public:
		TestLayer() 
			: m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
		{
			m_Lobby = std::make_unique<Lobby>();
		}

		void OnAttach() override {}

		void OnDetach() override {}
		
		void OnUpdate() override
		{
			Apex::Renderer::BeginScene(m_Camera);
			Apex::RenderCommands::Clear();
			for (auto& brick : m_Bricks)
				brick->Render();
			Apex::Renderer::EndScene();
		}
		
		void OnImGuiRender() override
		{
			static bool unselected = true;
			ImGui::Begin("Brick");
			//ImGui::DragFloat2("Position", &m_Lobby->GetPlayer(m_PlayerIndex)->GetPosition().x, 0.05f, -1.6f, 1.6f);

			if (unselected) {
				
				/* CLIENT */

				auto clientFun = [this](const Apex::Ref<Apex::TCPClientSocket>& socket, Apex::CommunicationPacket& packet) {
					MyClientPacket sendPacket;
					MyServerPacket recvPacket;
					sendPacket = { packet, m_PlayerIndex, m_Lobby->GetPlayer(m_PlayerIndex)->GetPosition() };
					//APEX_LOG_TRACE("[Send] Position :: {0} , {1}", sendPacket.position.x, sendPacket.position.y);
					if(socket->Send(sendPacket, sizeof(sendPacket)) < 1) return false;
					if(socket->Recv(recvPacket, sizeof(recvPacket)) < 1) return false;
					{
						uint32_t i = 0;
						for (auto& position : recvPacket.positions) {
							if (i != m_PlayerIndex)
								m_Bricks.at(i)->m_Position = position;
						}
					}
					//APEX_LOG_TRACE("[Recv] Position :: {0} , {1}", recvPacket.position.x, recvPacket.position.y);
					return true;
				};

				if (ImGui::Button("Client")) {
					Apex::NetworkManager::StartClientWorkerThread(std::make_shared<Apex::TCPClientSocket>("localhost", "27015"), clientFun, std::chrono::microseconds(25));
					unselected = false;
				}

				/* SERVER */

				auto serverFun = [this](const Apex::Ref<Apex::TCPClientSocket>& socket, Apex::CommunicationPacket& packet) {
					MyClientPacket sendPacket;
					MyServerPacket recvPacket;
					sendPacket = { packet, m_PlayerIndex, m_Lobby->GetPlayer(m_PlayerIndex)->GetPosition() };
					//APEX_LOG_TRACE("[Send] Position :: {0} , {1}", sendPacket.position.x, sendPacket.position.y);
					if (socket->Send(sendPacket, sizeof(sendPacket)) < 1) return false;
					if (socket->Recv(recvPacket, sizeof(recvPacket)) < 1) return false;
					{
						uint32_t i = 0;
						for (auto& position : recvPacket.positions) {
							if (i != m_PlayerIndex)
								m_Bricks.at(i)->m_Position = position;
						}
					}
					//APEX_LOG_TRACE("[Recv] Position :: {0} , {1}", recvPacket.position.x, recvPacket.position.y);
					return true;
				};

				if (ImGui::Button("Server")) {
					Apex::NetworkManager::StartServerWorkerThread(serverFun, std::chrono::microseconds(25));
					unselected = false;
				}
			}
			ImGui::End();
		}

	private:
		uint32_t m_PlayerIndex;
		Apex::Unique<Lobby> m_Lobby;

		std::vector<Apex::Unique<Brick>> m_Bricks;
		Apex::OrthographicCamera m_Camera;
	};
}