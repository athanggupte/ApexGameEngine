#include "apex_pch.h"
#include "CameraController.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Apex/Core/Input/Input.h"
#include "Apex/Core/Input/KeyCodes.h"
#include "Apex/Core/Input/MouseCodes.h"

namespace Apex {
	
	OrthographicCameraController2D::OrthographicCameraController2D(OrthographicCamera* camera, const glm::vec3& position, float rotation, float movementSpeed, float rotationSpeed)
		: m_CameraPosition(position), m_CameraRotation(rotation),
		m_MovementSpeed(movementSpeed), m_RotationSpeed(rotationSpeed)
	{
		m_Camera = camera;
	}
	
	void OrthographicCameraController2D::OnUpdate()
	{
		static std::pair<float, float> mousePos = { 0.f, 0.f };
		
		auto newMousePos = Input::GetMousePos();
		std::pair<float, float> mouseDiff = (Input::IsKeyPressed(APEX_KEY_LEFT_ALT) && Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_LEFT)) ? 
											std::pair{ (newMousePos.first - mousePos.first), (newMousePos.second - mousePos.second) } :
											std::pair{ 0.f, 0.f };
		
		m_CameraPosition.x -= mouseDiff.first * m_MovementSpeed;
		m_CameraPosition.y += mouseDiff.second * m_MovementSpeed;
		
		glm::vec3 startPos{ mousePos.first, mousePos.second, 0.f };
		glm::vec3 endPos{ newMousePos.first, newMousePos.second, 0.f };
		//m_CameraRotation += Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_RIGHT) ? (glm::acos(glm::dot(startPos, endPos)) * m_RotationSpeed) : 0.0f;
		
		mousePos = { newMousePos.first, newMousePos.second };
		
		m_Camera->SetPosition(m_CameraPosition);
		m_Camera->SetRotation(m_CameraRotation);
	}
	
	void OrthographicCameraController2D::OnEvent(Event& e) {}
	
}

