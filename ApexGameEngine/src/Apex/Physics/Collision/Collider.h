#pragma once

#include <glm/glm.hpp>

namespace Apex {

	class Collider
	{
	public:
		static enum class ColliderType
		{
			LINE = 1,
			AABB = 2, OBB,
			CIRCLE, CAPSULE
		};
	public:
		virtual ~Collider() = default;
		virtual bool IsColliding(const Ref<Collider>& collider) = 0;
		//virtual void Update() = 0;
		virtual ColliderType GetColliderType() const = 0;

	protected:
		/*static bool CheckCollisionLineLine(const Ref<LineCollider>& col1, const Ref<LineCollider>& col2);
		static bool CheckCollisionLineAABB(const Ref<LineCollider>& col1, const Ref<AABBCollider>& col2);
		static bool CheckCollisionAABBAABB(const Ref<AABBCollider>& col1, const Ref<AABBCollider>& col2);
		static bool CheckCollisionAABBCircle(const Ref<AABBCollider>& col1, const Ref<CircleCollider>& col2);
		static bool CheckCollisionCircleCircle(const Ref<CircleCollider>& col1, const Ref<CircleCollider>& col2);*/
		//static bool CheckCollision(const Ref<AABBCollider>& col1, const Ref<AABBCollider>& col2);
	};

	class LineCollider : Collider
	{
	public:
		LineCollider(glm::vec2 pointA, glm::vec2 pointB);
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
		bool IsColliding(const Ref<Collider>& collider) override;
		ColliderType GetColliderType() const override;
		
		const glm::vec2& GetCenter() const { return m_Center; }
		const glm::vec2& GetHalfDimensions() const { return m_HalfDimensions; }

		void SetCenter(const glm::vec2& center) { m_Center = center; }

		const std::pair<glm::vec2, glm::vec2>& GetCorners() const { return {m_Center - m_HalfDimensions, m_Center + m_HalfDimensions}; }
	private:
		glm::vec2 m_Center, m_HalfDimensions;
	};

	class CircleCollider : public Collider
	{
	public:
		CircleCollider(glm::vec2 centre, float radius);
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
		static bool CheckCollisionLineLine(const Ref<LineCollider>& col1, const Ref<LineCollider>& col2) {}
		static bool CheckCollisionLineAABB(const Ref<LineCollider>& col1, const Ref<AABBCollider>& col2);
		static bool CheckCollisionAABBAABB(const Ref<AABBCollider>& col1, const Ref<AABBCollider>& col2);
		static bool CheckCollisionAABBCircle(const Ref<AABBCollider>& col1, const Ref<CircleCollider>& col2) {}
		static bool CheckCollisionCircleCircle(const Ref<CircleCollider>& col1, const Ref<CircleCollider>& col2);
	};
}