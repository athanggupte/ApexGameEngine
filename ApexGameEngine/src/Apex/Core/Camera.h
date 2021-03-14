#pragma once

#include <glm/glm.hpp>

namespace Apex {
	
	class Camera
	{
	public:
		virtual const glm::mat4& GetProjectionMatrix() const = 0;
		virtual const glm::mat4& GetViewMatrix() const = 0;
		virtual const glm::mat4& GetViewProjectionMatrix() const = 0;
	
	protected:
		virtual void RecalculateViewMatrix() = 0;
	};

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float nearZ = -1.0f, float farZ = 10.0f);

		void LookAt(const glm::vec3& center);

		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }
		void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix();  }

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline float GetRotation() const { return m_Rotation; }

		inline const glm::mat4& GetProjectionMatrix() const override { return m_ProjectionMatrix; }
		inline const glm::mat4& GetViewMatrix() const override { return m_ViewMatrix; }
		inline const glm::mat4& GetViewProjectionMatrix() const override { return m_ViewProjectionMatrix; }

	private:
		void RecalculateViewMatrix() override;

	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = { 0.f, 0.f, 0.f };
		float m_Rotation = 0.f;
	};

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float fovAngle, float aspectRatio, float nearZ = 0.5f, float farZ = 500.0f);

		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }
		void LookAt(glm::vec3 center);
		//void SetWorldUp(const glm::vec3& worldUp) { m_WorldUp = worldUp; consistent = false; }

		void Move(const glm::vec3& movement);
		void Rotate(float pitch, float yaw, float roll);

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline const glm::vec3 GetRotation() const { glm::vec3 rot(m_Pitch, m_Yaw, m_Roll); return rot; }

		inline const glm::vec3& GetFront() const { return m_Front; }
		inline const glm::vec3& GetRight() const { return m_Right; }
		inline const glm::vec3& GetUp() const { return m_Up; }

		inline const glm::mat4& GetProjectionMatrix() const override { return m_ProjectionMatrix; }
		inline const glm::mat4& GetViewMatrix() const override { return m_ViewMatrix; }
		inline const glm::mat4& GetViewProjectionMatrix() const override { return m_ViewProjectionMatrix; }

	private:
		void RecalculateViewMatrix() override;

	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = { 0.f, 0.f, 0.f };
		glm::vec3 m_WorldUp = { 0.f, 1.f, 0.f };
		
		glm::vec3 m_Front = { 0.f, 0.f, -1.f };
		glm::vec3 m_Right = { 1.f, 0.f, 0.f };
		glm::vec3 m_Up = { 0.f, 1.f, 0.f };

		float m_Pitch = 0.f, m_Yaw = -90.f, m_Roll = 0.f;

		bool consistent = false;
	};

}
