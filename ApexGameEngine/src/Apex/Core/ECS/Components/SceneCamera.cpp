#include "apex_pch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Apex {
	
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}
	SceneCamera::SceneCamera(ProjectionType projType)
		: m_ProjectionType(projType)
	{
		RecalculateProjection();
	}
	
	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}
	
	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
		RecalculateProjection();
	}
	
	void SceneCamera::SetPerspective(float fov, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFovY = fov;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
		RecalculateProjection();
	}
	
	void SceneCamera::RecalculateProjection()
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
