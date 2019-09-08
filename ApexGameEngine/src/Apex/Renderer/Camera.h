#pragma once

#include <glm/glm.hpp>

namespace Apex {
	
	class APEX_API Camera
	{
	public:
		virtual const glm::mat4& GetProjectionMatrix() const = 0;
		virtual const glm::mat4& GetViewMatrix() const = 0;
		virtual const glm::mat4& GetViewProjectionMatrix() const = 0;
	
	protected:
		virtual void RecalculateViewMatrix() = 0;
	};

	class APEX_API OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }
		void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix();  }

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline float GetRotation() const { return m_Rotation; }

		inline const glm::mat4& GetProjectionMatrix() const override { return m_ProjectionMatrix; }
		inline const glm::mat4& GetViewMatrix() const override { return m_ViewMatrix; }
		inline const glm::mat4& GetViewProjectionMatrix() const override { return m_ViewProjectionMatrix; }

	private:
		void RecalculateViewMatrix();

	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = { 0.f, 0.f, 0.f };
		float m_Rotation = 0.f;
	};

}