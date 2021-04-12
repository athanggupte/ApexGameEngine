#pragma once

#include <glm/glm.hpp>

namespace Apex {

	/// Collision ///
	class Collision
	{
	public:
		static void Init() { s_Instance = new Collision(); }

		static const glm::vec2& GetNormal() { return s_Instance->normal; }
		static float GetRestitution() { return s_Instance->e; }

		static void SetNormal(const glm::vec2& normal) { s_Instance->normal = normal; }
		static void SetRestitution(float e) { s_Instance->e = e; }

	public:
		float e = 1.0f;
		glm::vec2 position;
		glm::vec2 normal;
	private:
		static Collision* s_Instance;
	};

	/// Collider ///
	class Collider
	{
	public:
		enum class ColliderType
		{
			LINE = 1,
			AABB = 2, OBB,
			CIRCLE, CAPSULE
		};
	public:
		virtual ~Collider() = default;
		//virtual void Update() = 0;
		virtual bool IsUnderMouse(const glm::vec2& mousePos) = 0;
		virtual bool IsColliding(const Ref<Collider>& collider) = 0;
		virtual ColliderType GetColliderType() const = 0;
	};

	class LineCollider : Collider
	{
	public:
		LineCollider(glm::vec2 pointA, glm::vec2 pointB);
		bool IsUnderMouse(const glm::vec2& mousePos) override;
		bool IsColliding(const Ref<Collider>& collider) override;
		ColliderType GetColliderType() const override;

		const glm::vec2& GetPoint() const { return m_Point; }
		const glm::vec2& GetDirection() const { return m_Direction; }
	private:
		glm::vec2 m_Point;
		glm::vec2 m_Direction;
	};

	class AABBCollider : public Collider
	{
	public:
		AABBCollider(glm::vec2 topLeft, glm::vec2 bottomRight);
		bool IsUnderMouse(const glm::vec2& mousePos) override;
		bool IsColliding(const Ref<Collider>& collider) override;
		ColliderType GetColliderType() const override;
		
		const glm::vec2& GetCenter() const { return m_Center; }
		const glm::vec2& GetHalfDimensions() const { return m_HalfDimensions; }

		void SetCenter(const glm::vec2& center) { m_Center = center; }

		const std::pair<glm::vec2, glm::vec2> GetCorners() const { return { {m_Center.x - m_HalfDimensions.x, m_Center.y + m_HalfDimensions.y}, {m_Center.x + m_HalfDimensions.x, m_Center.y - m_HalfDimensions.y} }; }
	private:
		glm::vec2 m_Center, m_HalfDimensions;
	};

	class CircleCollider : public Collider
	{
	public:
		CircleCollider(glm::vec2 centre, float radius);
		bool IsUnderMouse(const glm::vec2& mousePos) override;
		bool IsColliding(const Ref<Collider>& collider) override;
		ColliderType GetColliderType() const override;

		const glm::vec2& GetCenter() const { return m_Center; }
		const float& GetRadius() const { return m_Radius; }

		void SetCenter(const glm::vec2& center) { m_Center = center; }
	private:
		glm::vec2 m_Center;
		float m_Radius;
	};

	class CollisionCheck {
	public:
		static bool CheckCollisionLineLine(const Ref<LineCollider>& col1, const Ref<LineCollider>& col2) { return false; }
		static bool CheckCollisionLineAABB(const Ref<LineCollider>& col1, const Ref<AABBCollider>& col2);
		static bool CheckCollisionAABBAABB(const Ref<AABBCollider>& col1, const Ref<AABBCollider>& col2);
		static bool CheckCollisionAABBCircle(const Ref<AABBCollider>& col1, const Ref<CircleCollider>& col2);
		static bool CheckCollisionCircleCircle(const Ref<CircleCollider>& col1, const Ref<CircleCollider>& col2);
	};
}
