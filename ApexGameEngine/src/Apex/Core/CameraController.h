#pragma once

#include "Camera.h"

#include <glm/glm.hpp>

#include "Apex/Core/Timer.h"
#include "Apex/Core/Events/Event.h"
#include "Apex/Core/Events/ApplicationEvent.h"
#include "Apex/Core/Events/MouseEvent.h"

namespace Apex {

	class CameraController
	{
	public:
		CameraController() = default;
		CameraController(const CameraController&) = default;
		
		virtual const Camera& GetCamera() const = 0;
		
		virtual void OnUpdate(Timestep ts) = 0;
		virtual void OnEvent(Event&) = 0;
		
		virtual void OnResize(float width, float height) = 0;
		
	private:
		//Camera* m_Camera;
	};

	class OrthographicCameraController2D : public CameraController
	{
	public:
		OrthographicCameraController2D(float aspectRatio,
							float zoomLevel = 1.f,
							const glm::vec3& position = { 0.f, 0.f, 0.f },
							float rotation = 0.f,
							float movementSpeed = 0.0025f,
							float rotationSpeed = 1.f);
		
		virtual const Camera& GetCamera() const override { return m_Camera; }
		
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnEvent(Event&) override;
		
		virtual void OnResize(float width, float height) override;
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		
	private:
		// State
		float m_AspectRatio;
		float m_ZoomLevel;
		glm::vec3 m_CameraPosition;
		float m_CameraRotation;
		bool m_IsDragging = false;
		std::pair<float, float> m_DragStartPos = { 0.f, 0.f };
		
		// Camera Handle
		OrthographicCamera m_Camera;
		
		// Modifiers
		float m_MovementSpeed;
		float m_RotationSpeed;
	};

// 	class CamerController2D : public CameraController
// 	{
// 		
// 	};

// 	class ThirdPersonCameraController : public CameraController
// 	{
// 		
// 	};

// 	class FirstPersonCameraController : public CameraController
// 	{
// 		
// 	};
	
}
