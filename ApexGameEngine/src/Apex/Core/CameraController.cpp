#include "apex_pch.h"
#include "CameraController.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Apex/Core/Input/Input.h"
#include "Apex/Core/Input/KeyCodes.h"
#include "Apex/Core/Input/MouseCodes.h"

#include "Apex/Utils/MathPrimitiveParser.h"

namespace Apex {

	////////////////////////////////////////////////////////
	////////////// Orthographic Controller /////////////////
	////////////////////////////////////////////////////////

	OrthographicCameraController2D::OrthographicCameraController2D(Camera& camera,
		float zoomLevel, const glm::vec3& position, float rotation, float movementSpeed, float rotationSpeed)
		: CameraController(camera), m_ZoomLevel(zoomLevel),
		m_CameraPosition(position), m_CameraRotation(rotation),
		m_MovementSpeed(movementSpeed), m_RotationSpeed(rotationSpeed)
	{
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

	
	////////////////////////////////////////////////////////
	/////////////// Perspective Controller /////////////////
	////////////////////////////////////////////////////////

	constexpr static glm::vec3 worldUp = { 0.f, 1.f, 0.f };

	PerspectiveCameraController::PerspectiveCameraController(Camera& camera, const glm::vec3& camera_position,
		const glm::vec3& camera_rotation, float movement_speed, float rotation_speed)
		: CameraController(camera), m_CameraPosition(camera_position),
		m_MovementSpeed(movement_speed), m_RotationSpeed(rotation_speed)
	{
		m_CameraDirection = { 0.f, 0.f, 1.f };
	}

	glm::mat4 PerspectiveCameraController::GetTransform() const
	{
		glm::mat4 rotation = glm::mat4{
			{ m_CameraRight.x, m_CameraUp.x, m_CameraDirection.x, 0.f },
			{ m_CameraRight.y, m_CameraUp.y, m_CameraDirection.y, 0.f },
			{ m_CameraRight.z, m_CameraUp.z, m_CameraDirection.z, 0.f },
			{ 0.f, 0.f, 0.f, 1.f  },
		};
		rotation = glm::transpose(rotation);

		return glm::translate(glm::mat4(1.f), m_CameraPosition) * rotation;
	}

	void PerspectiveCameraController::OnUpdate(Timestep ts)
	{
		glm::vec3 localDisplacement{ 0.f };
		glm::vec3 localRotation{ 0.f };
		bool changed = false;

		if (Input::IsKeyPressed(APEX_KEY_W))
		{
			localDisplacement.z += m_MovementSpeed * ts;
			changed = true;
		}
		if (Input::IsKeyPressed(APEX_KEY_S))
		{
			localDisplacement.z -= m_MovementSpeed * ts;
			changed = true;
		}
		if (Input::IsKeyPressed(APEX_KEY_A))
		{
			localDisplacement.x -= m_MovementSpeed * ts;
			changed = true;
		}
		if (Input::IsKeyPressed(APEX_KEY_D))
		{
			localDisplacement.x += m_MovementSpeed * ts;
			changed = true;
		}
		if (Input::IsKeyPressed(APEX_KEY_E))
		{
			localDisplacement.y += m_MovementSpeed * ts;
			changed = true;
		}
		if (Input::IsKeyPressed(APEX_KEY_Q))
		{
			localDisplacement.y -= m_MovementSpeed * ts;
			changed = true;
		}

		if (Input::IsKeyPressed(APEX_KEY_UP))
		{
			localRotation.x += m_RotationSpeed * ts;
			changed = true;
		}
		if (Input::IsKeyPressed(APEX_KEY_DOWN))
		{
			localRotation.x -= m_RotationSpeed * ts;
			changed = true;
		}
		if (Input::IsKeyPressed(APEX_KEY_LEFT))
		{
			localRotation.y += m_RotationSpeed * ts;
			changed = true;
		}
		if (Input::IsKeyPressed(APEX_KEY_RIGHT))
		{
			localRotation.y -= m_RotationSpeed * ts;
			changed = true;
		}


		// If there is no displacement then don't do costly calculation
		if (!changed)
			return;

		glm::vec3 prevDir = m_CameraDirection;
		glm::vec3 prevRight = m_CameraRight;
		glm::vec3 prevUp = m_CameraUp;

		m_CameraDirection = (glm::rotate(glm::mat4(1.f), localRotation.x, prevRight) 
							* glm::rotate(glm::mat4(1.f), localRotation.y, prevUp)
							* glm::vec4(prevDir, 0.f));
		m_CameraRight = glm::normalize(glm::cross(worldUp, m_CameraDirection));
		m_CameraUp = glm::cross(m_CameraDirection, m_CameraRight);

		m_CameraPosition += glm::mat3{ m_CameraRight, m_CameraUp, -m_CameraDirection } * localDisplacement;
	}

	void PerspectiveCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>([this] (MouseScrolledEvent& e)
		{
			static float scrollTotal = 0;
			scrollTotal += e.GetOffsetY();
			scrollTotal = glm::clamp(scrollTotal, -10.f, 14.f);
			m_MovementSpeed = glm::pow(1.25f, scrollTotal);
			m_MovementSpeed = glm::clamp(m_MovementSpeed, 0.125f, 16.f);

			APEX_CORE_TRACE("scroll Total: {0} | MovementSpeed: {1}", scrollTotal, m_MovementSpeed);

			return false;
		});
	}

	void PerspectiveCameraController::OnResize(uint32_t width, uint32_t height)
	{
		m_Camera->SetViewportSize(width, height);
	}

	void PerspectiveCameraController::LookAt(const glm::vec3& target)
	{
		auto lookAtMat = glm::lookAt(m_CameraPosition, target, worldUp);
		APEX_CORE_DEBUG("{}", MathParser::ParseMatrix(glm::inverse(lookAtMat)));

		m_CameraDirection = glm::normalize(m_CameraPosition - target);
		m_CameraRight = glm::normalize(glm::cross(worldUp, m_CameraDirection));
		m_CameraUp = glm::cross(m_CameraDirection, m_CameraRight);
	}

	void PerspectiveCameraController::FocusAt(const glm::vec3& target_position, const glm::vec3& viewing_direction,
	                                          const float& distance)
	{
	}
}

