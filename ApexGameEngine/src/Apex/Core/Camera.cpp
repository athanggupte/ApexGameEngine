#include "apex_pch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Apex {

	//////////////////////////////////////////////////////////////////////
	/*----------------------Orthographic Camera-------------------------*/
	//////////////////////////////////////////////////////////////////////

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float nearZ, float farZ)
		: Camera(glm::ortho(left, right, bottom, top, nearZ, farZ), glm::mat4(1.0f))
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::LookAt(const glm::vec3& center)
	{
		m_ViewMatrix = glm::lookAt(m_Position, center, glm::vec3(0.0f, 1.0f, 0.0f));
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}



	//////////////////////////////////////////////////////////////////////
	/*----------------------Perspective Camera--------------------------*/
	//////////////////////////////////////////////////////////////////////

	PerspectiveCamera::PerspectiveCamera(float fovAngle, float aspectRatio, float nearZ, float farZ)
		: Camera(glm::perspective((float)glm::radians(fovAngle), aspectRatio, nearZ, farZ), glm::mat4(1.0f))
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void PerspectiveCamera::LookAt(glm::vec3 center)
	{
		m_ViewMatrix = glm::lookAt(m_Position, center, m_WorldUp);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void PerspectiveCamera::Move(const glm::vec3 & movement)
	{
		m_Position -= m_Front * movement.z;
		m_Position += m_Right * movement.x;
		m_Position += m_Up * movement.y;
		RecalculateViewMatrix();
	}

	void PerspectiveCamera::Rotate(float pitch, float yaw, float roll)
	{
		m_Pitch += pitch;
		m_Yaw += yaw;
		m_Roll += roll;
		
		if (m_Pitch > 360.f || m_Pitch < -360.f)
			m_Pitch = 0.f;
		if (m_Yaw > 360.f || m_Yaw < -360.f)
			m_Yaw = 0.f;

		RecalculateViewMatrix();
	}

	void PerspectiveCamera::RecalculateViewMatrix()
	{
		m_Front.x = cos(glm::radians(m_Pitch)) * cos(glm::radians(m_Yaw));
		m_Front.z = cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));
		m_Front.y = sin(glm::radians(m_Pitch));

		m_Front = glm::normalize(m_Front);
		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));

		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}
