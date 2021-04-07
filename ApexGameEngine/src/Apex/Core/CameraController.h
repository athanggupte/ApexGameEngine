#pragma once

#include "Camera.h"

#include <glm/glm.hpp>

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
		
		virtual void OnUpdate() = 0;
		virtual void OnEvent(Event&) = 0;
		
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
		
		virtual void OnUpdate() override;
		virtual void OnEvent(Event&) override;
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		
	private:
		// State
		float m_AspectRatio;
		float m_ZoomLevel;
		glm::vec3 m_CameraPosition;
		float m_CameraRotation;
		
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
