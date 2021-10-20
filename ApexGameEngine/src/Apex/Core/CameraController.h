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
		CameraController(Camera& camera)
			: m_Camera(&camera) {}

		CameraController(const CameraController&) = default;
		
		const Camera& GetCamera() const { return *m_Camera; }

		// Pure Virtual Functions
		virtual glm::mat4 GetTransform() const = 0;
		virtual void OnUpdate(Timestep ts) = 0;
		virtual void OnEvent(Event&) = 0;
		virtual void OnResize(uint32_t width, uint32_t height) = 0;

	protected:
		// Camera Handle
		Camera* m_Camera;
	};

	class OrthographicCameraController2D : public CameraController
	{
	public:
		OrthographicCameraController2D(Camera& camera,
							float zoomLevel = 1.f,
							const glm::vec3& position = { 0.f, 0.f, 0.f },
							float rotation = 0.f,
							float movementSpeed = 0.0025f,
							float rotationSpeed = 1.f);
		
		glm::mat4 GetTransform() const override;
		void OnUpdate(Timestep ts) override;
		void OnEvent(Event&) override;
		void OnResize(uint32_t width, uint32_t height) override;

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	
	private:
		// State
		float m_ZoomLevel;
		glm::vec3 m_CameraPosition;
		float m_CameraRotation;
		bool m_IsDragging = false;
		std::pair<float, float> m_DragStartPos = { 0.f, 0.f };
		
		// Modifiers
		float m_MovementSpeed;
		float m_RotationSpeed;
	};

	class PerspectiveCameraController : public CameraController
	{
	public:
	private:
		// Modifiers
		float m_MovementSpeed;
		float m_RotationSpeed;
	};
	
}
