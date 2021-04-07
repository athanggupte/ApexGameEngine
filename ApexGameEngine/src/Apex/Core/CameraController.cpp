#include "apex_pch.h"
#include "CameraController.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Apex/Core/Input/Input.h"
#include "Apex/Core/Input/KeyCodes.h"
#include "Apex/Core/Input/MouseCodes.h"

#include "Apex/Utils/MathPrimitiveParser.h"

namespace Apex {
	
	OrthographicCameraController2D::OrthographicCameraController2D(float aspectRatio, float zoomLevel,
																   const glm::vec3& position, float rotation, float movementSpeed, float rotationSpeed)
		: m_AspectRatio(aspectRatio), m_ZoomLevel(zoomLevel),
		m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
		m_CameraPosition(position), m_CameraRotation(rotation),
		m_MovementSpeed(movementSpeed), m_RotationSpeed(rotationSpeed)
	{
		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);
	}
	
	void OrthographicCameraController2D::OnUpdate()
	{
		static std::pair<float, float> mousePos = { 0.f, 0.f };
		
		auto newMousePos = Input::GetMousePos();
		std::pair<float, float> mouseDiff = (Input::IsKeyPressed(APEX_KEY_LEFT_ALT) && Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_LEFT)) ? 
											std::pair{ (newMousePos.first - mousePos.first), (newMousePos.second - mousePos.second) } :
											std::pair{ 0.f, 0.f };
		
		m_CameraPosition.x -= mouseDiff.first * m_MovementSpeed * m_ZoomLevel;
		m_CameraPosition.y += mouseDiff.second * m_MovementSpeed * m_ZoomLevel;
		
		glm::vec3 startPos{ mousePos.first, mousePos.second, 0.f };
		glm::vec3 endPos{ newMousePos.first, newMousePos.second, 0.f };
		//m_CameraRotation += Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_RIGHT) ? (glm::acos(glm::dot(startPos, endPos)) * m_RotationSpeed) : 0.0f;
		
		mousePos = { newMousePos.first, newMousePos.second };
		
		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);
	}
	
	void OrthographicCameraController2D::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(APEX_BIND_EVENT_FN(OrthographicCameraController2D::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(APEX_BIND_EVENT_FN(OrthographicCameraController2D::OnWindowResize));
	}
	
	bool OrthographicCameraController2D::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomLevel -= e.GetOffsetY() * 0.25f;
		m_ZoomLevel = glm::clamp(m_ZoomLevel, 0.125f, 8.f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
	
	bool OrthographicCameraController2D::OnWindowResize(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
}

