#pragma once

#include <glm/glm.hpp>

namespace Apex {

	class APEX_API Camera
	{
	public:
		Camera(const glm::vec3& position, const glm::vec3& rotation, float fov, float aspectRatio, float nearPlane, float farPlane);

		inline const float GetFOV() const { return m_FOV; }
		inline const float GetAspectRatio() const { return m_AspectRatio; }

	private:
		glm::vec3 m_Position;
		glm::vec3 m_Rotation;
		float m_FOV;
		float m_AspectRatio;
		float m_NearPlane;
		float m_FarPlane;
	};

	class APEX_API OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }
		void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix();  }

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline float GetRotation() const { return m_Rotation; }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	private:
		void RecalculateViewMatrix();

	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position;
		float m_Rotation;
	};

}