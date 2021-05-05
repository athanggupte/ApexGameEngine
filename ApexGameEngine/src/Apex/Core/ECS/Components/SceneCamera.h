#pragma once

#include <glm/glm.hpp>

namespace Apex {
	
	class SceneCamera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };
	public:
		SceneCamera();
		SceneCamera(ProjectionType projType);
		// SceneCamera(const glm::mat4& projection) : m_Projection(projection) {}
		virtual ~SceneCamera() = default;
		
		const glm::mat4& GetProjection() const { return m_Projection; }
		
		void SetViewportSize(uint32_t width, uint32_t height);
		
		void SetOrthographic(float size, float nearClip, float farClip);
		void SetPerspective(float fov, float nearClip, float farClip);
		
		void SetProjectionType(ProjectionType projType) { m_ProjectionType = projType; }
		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		
		// Orthographic Properties
		float GetOrthographicSize() const { return m_OrthographicSize; }
		float GetOrthographicNear() const { return m_OrthographicNear; }
		float GetOrthographicFar() const { return m_OrthographicFar; }
		
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
		void SetOrthographicNear(float near) { m_OrthographicNear = near; RecalculateProjection(); }
		void SetOrthographicFar(float far) { m_OrthographicFar = far; RecalculateProjection(); }
		
		// Perspective Properties
		float GetPerspectiveVerticalFov() const { return m_PerspectiveFovY; }
		float GetPerspectiveNear() const { return m_PerspectiveNear; }
		float GetPerspectiveFar() const { return m_PerspectiveFar; }
		
		void SetPerspectiveVerticalFov(float fovy) { m_PerspectiveFovY = fovy; RecalculateProjection(); }
		void SetPerspectiveNear(float near) { m_PerspectiveNear = near; RecalculateProjection(); }
		void SetPerspectiveFar(float far) { m_PerspectiveFar = far; RecalculateProjection(); }
		
	private:
		void RecalculateProjection();
		
	private:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;
		
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
	
	
}
