#include "apex_pch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Apex {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		:// m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
		m_ProjectionMatrix(glm::perspective((float)glm::radians(63.5), 16.f/9.f, 0.5f, 100.0f))
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}