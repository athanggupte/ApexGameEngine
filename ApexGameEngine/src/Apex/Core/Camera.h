#pragma once

#include <glm/glm.hpp>

namespace Apex {

	class Camera
	{
	public:
		enum class ProjectionType
		{
			Perspective = 0, Orthographic = 1,

			_COUNT
		};
	public:
		// Camera();
		Camera(ProjectionType projType);
		Camera(ProjectionType projType, uint32_t viewportWidth, uint32_t viewportHeight);
		virtual ~Camera() = default;
		
		const glm::mat4& GetProjection() const { return m_Projection; }
		
		void SetViewportSize(uint32_t width, uint32_t height);
		void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; RecalculateProjection(); }
		
		void SetOrthographic(float size, float nearClip, float farClip);
		void SetPerspective(float fov, float nearClip, float farClip);
		
		void SetProjectionType(ProjectionType projType) { m_ProjectionType = projType; }
		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		
		// Orthographic Properties
		float GetOrthographicSize() const { return m_OrthographicSize; }
		float GetOrthographicNear() const { return m_OrthographicNear; }
		float GetOrthographicFar() const { return m_OrthographicFar; }
		
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
		void SetOrthographicNear(float _near) { m_OrthographicNear = _near; RecalculateProjection(); }
		void SetOrthographicFar(float _far) { m_OrthographicFar = _far; RecalculateProjection(); }
		
		// Perspective Properties
		float GetPerspectiveVerticalFov() const { return m_PerspectiveFovY; }
		float GetPerspectiveNear() const { return m_PerspectiveNear; }
		float GetPerspectiveFar() const { return m_PerspectiveFar; }
		
		void SetPerspectiveVerticalFov(float fovy) { m_PerspectiveFovY = fovy; RecalculateProjection(); }
		void SetPerspectiveNear(float _near) { m_PerspectiveNear = _near; RecalculateProjection(); }
		void SetPerspectiveFar(float _far) { m_PerspectiveFar = _far; RecalculateProjection(); }
		
	private:
		void RecalculateProjection();
		
	private:
		ProjectionType m_ProjectionType;
		
		// Orthographic Properties
		// OrthographicProps m_OrthographicProps;
		float m_OrthographicSize = 10.f;
		float m_OrthographicNear = -1.f;
		float m_OrthographicFar = 1.f;
		
		// Perspective properties
		// PerspectiveProps m_PerspectiveProps;
		float m_PerspectiveFovY = glm::radians(60.f);
		float m_PerspectiveNear = 0.01f;
		float m_PerspectiveFar = 1000.f;
		
		float m_AspectRatio = 0.f;
		
		glm::mat4 m_Projection = glm::mat4(1.f);
	};
	
	constexpr const char* CameraProjectionTypeString(Camera::ProjectionType type)
	{
		switch (type)
		{
		case Camera::ProjectionType::Perspective: return "Perspective";
		case Camera::ProjectionType::Orthographic: return "Orthographic";
		default: return "UNKNOWN";
		}
	}

}
