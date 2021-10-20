#include "apex_pch.h"
#include "CameraController.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Apex/Core/Input/Input.h"
#include "Apex/Core/Input/KeyCodes.h"
#include "Apex/Core/Input/MouseCodes.h"

#include "Apex/Utils/MathPrimitiveParser.h"

namespace Apex {
	
	OrthographicCameraController2D::OrthographicCameraController2D(Camera& camera,
		float zoomLevel, const glm::vec3& position, float rotation, float movementSpeed, float rotationSpeed)
		: CameraController(camera), m_ZoomLevel(zoomLevel),
		m_CameraPosition(position), m_CameraRotation(rotation),
		m_MovementSpeed(movementSpeed), m_RotationSpeed(rotationSpeed)
	{
		m_Camera->SetProjectionType(Camera::ProjectionType::Orthographic);
		m_Camera->SetOrthographicSize(m_ZoomLevel * 2.f);
	}

	glm::mat4 OrthographicCameraController2D::GetTransform() const
	{
		return glm::translate(glm::mat4(1.0f), m_CameraPosition)
			* glm::rotate(glm::mat4(1.0f), glm::radians(m_CameraRotation), { 0.f, 0.f, 1.f });
	}

	void OrthographicCameraController2D::OnUpdate(Timestep ts)
	{
		if (m_IsDragging) {
			if (!Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_LEFT)) {
				m_IsDragging = false;
				return;
			}
			auto newMousePos = Input::GetMousePos();
			std::pair<float, float> mouseDiff = std::pair{ (newMousePos.first - m_DragStartPos.first), (newMousePos.second - m_DragStartPos.second) };
												//std::pair{ 0.f, 0.f };
			
			m_CameraPosition.x -= mouseDiff.first * m_MovementSpeed * m_ZoomLevel;
			m_CameraPosition.y += mouseDiff.second * m_MovementSpeed * m_ZoomLevel;
			
// 			glm::vec3 startPos{ mousePos.first, mousePos.second, 0.f };
// 			glm::vec3 endPos{ newMousePos.first, newMousePos.second, 0.f };
			//m_CameraRotation += Input::IsMouseButtonPressed(APEX_MOUSE_BUTTON_RIGHT) ? (glm::acos(glm::dot(startPos, endPos)) * m_RotationSpeed) : 0.0f;
			
			m_DragStartPos = { newMousePos.first, newMousePos.second };
		}
	}
	
	void OrthographicCameraController2D::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseButtonPressedEvent>(APEX_BIND_EVENT_FN(OrthographicCameraController2D::OnMouseButtonPressedEvent));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(APEX_BIND_EVENT_FN(OrthographicCameraController2D::OnMouseButtonReleasedEvent));
		dispatcher.Dispatch<MouseScrolledEvent>(APEX_BIND_EVENT_FN(OrthographicCameraController2D::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(APEX_BIND_EVENT_FN(OrthographicCameraController2D::OnWindowResize));
	}

	void OrthographicCameraController2D::OnResize(uint32_t width, uint32_t height)
	{
		m_Camera->SetViewportSize(width, height);
	}
	
	bool OrthographicCameraController2D::OnMouseScrolled(MouseScrolledEvent& e)
	{
		APEX_CORE_DEBUG("OrthographicCameraController2D:: MouseScrolled");
		m_ZoomLevel -= e.GetOffsetY() * 0.25f;
		m_ZoomLevel = glm::clamp(m_ZoomLevel, 0.125f, 8.f);
		m_Camera->SetOrthographicSize(m_ZoomLevel * 2.f);
		return false;
	}
	
	bool OrthographicCameraController2D::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == APEX_MOUSE_BUTTON_LEFT) {
			m_IsDragging = true;
			m_DragStartPos = Input::GetMousePos();
		}
		return false;
	}
	
	bool OrthographicCameraController2D::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
	{
		if (e.GetMouseButton() == APEX_MOUSE_BUTTON_LEFT)
			m_IsDragging = false;
		return false;
	}
	
	bool OrthographicCameraController2D::OnWindowResize(WindowResizeEvent& e)
	{
		OnResize(e.GetWidth(), e.GetHeight());
		return false;
	}
}

