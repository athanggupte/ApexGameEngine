#include "apex_pch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Apex {

	Camera::Camera(ProjectionType projType)
		: m_ProjectionType(projType)
	{
		RecalculateProjection();
	}

	Camera::Camera(ProjectionType projType, uint32_t viewportWidth, uint32_t viewportHeight)
		: m_ProjectionType(projType), m_AspectRatio(static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight))
	{
		RecalculateProjection();
	}

	void Camera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
		RecalculateProjection();
	}
	
	void Camera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
		RecalculateProjection();
	}
	
	void Camera::SetPerspective(float fov, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFovY = fov;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
		RecalculateProjection();
	}
	
	void Camera::RecalculateProjection()
	{
		m_Projection = (m_ProjectionType == ProjectionType::Perspective)
		
						? glm::perspective(m_PerspectiveFovY, 
										   m_AspectRatio,
										   m_PerspectiveNear,
										   m_PerspectiveFar)
		
						: glm::ortho(-m_OrthographicSize/2.f * m_AspectRatio,
									 m_OrthographicSize/2.f * m_AspectRatio,
									 -m_OrthographicSize/2.f,
									 m_OrthographicSize/2.f,
									 m_OrthographicNear,
									 m_OrthographicFar);
	}
	
}
