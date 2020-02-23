#include "apex_pch.h"
#include "Collider.h"

#include <glm/gtx/perpendicular.hpp>

namespace Apex {


	////////////////////////////////////////////////
	/////////////////// Collider ///////////////////
	////////////////////////////////////////////////
	bool CollisionCheck::CheckCollisionLineAABB(const Ref<LineCollider>& colLine, const Ref<AABBCollider>& colBox)
	{
		colLine->GetPoint();
		auto[min, max] = colBox->GetCorners();
		return false;
	}

	bool CollisionCheck::CheckCollisionAABBAABB(const Ref<AABBCollider>& colBox1, const Ref<AABBCollider>& colBox2)
	{
		bool collisionX = ((colBox1->GetCenter().x + colBox1->GetHalfDimensions().x) >= (colBox2->GetCenter().x - colBox2->GetHalfDimensions().x)
			&& (colBox2->GetCenter().x + colBox2->GetHalfDimensions().x) >= (colBox1->GetCenter().x - colBox1->GetHalfDimensions().x));
		bool collisionY = ((colBox1->GetCenter().y + colBox1->GetHalfDimensions().y) >= (colBox2->GetCenter().y - colBox2->GetHalfDimensions().y)
			&& (colBox2->GetCenter().y + colBox2->GetHalfDimensions().y) >= (colBox1->GetCenter().y - colBox1->GetHalfDimensions().y));
		return collisionX && collisionY;
	}

	bool CollisionCheck::CheckCollisionCircleCircle(const Ref<CircleCollider>& col1, const Ref<CircleCollider>& col2)
	{
		auto dist = glm::distance(col1->GetCenter(), col2->GetCenter());
		auto sum_rad = col1->GetRadius() + col2->GetRadius();
		//APEX_CORE_DEBUG("Circle-Circle Collision \t|| distance : {0} \t|| sum(radii) : {1}", dist, sum_rad);
		return  dist < sum_rad;
	}

	/////////////////////////////////////////////////////
	/////////////////// AABB Collider ///////////////////
	/////////////////////////////////////////////////////
	AABBCollider::AABBCollider(glm::vec2 topLeft, glm::vec2 bottomRight)
	{
		m_Center = (topLeft + bottomRight) / 2.0f;
		m_HalfDimensions = glm::abs(topLeft - bottomRight) / 2.0f;
	}
	
	bool AABBCollider::IsColliding(const Ref<Collider>& collider)
	{
		switch (collider->GetColliderType())
		{
		case ColliderType::LINE:			return false;//CollisionCheck::CheckCollisionLineAABB(std::make_shared<LineCollider>(collider), std::make_shared<AABBCollider>(this));
		case ColliderType::AABB:			return false;//CollisionCheck::CheckCollisionAABBAABB(std::make_shared<AABBCollider>(this), std::make_shared<AABBCollider>(collider));
		case ColliderType::OBB:				return false;
		case ColliderType::CIRCLE:			return false;
		case ColliderType::CAPSULE:			return false;
		default:							return false;
		}
	}
	
	Collider::ColliderType AABBCollider::GetColliderType() const
	{
		return ColliderType::AABB;
	}

	///////////////////////////////////////////////////////
	/////////////////// Circle Collider ///////////////////
	///////////////////////////////////////////////////////
	CircleCollider::CircleCollider(glm::vec2 center, float radius)
		: m_Center(center), m_Radius(radius)
	{
	}

	bool CircleCollider::IsColliding(const Ref<Collider>& collider)
	{
		switch (collider->GetColliderType())
		{
		case ColliderType::LINE:			return false;
		case ColliderType::AABB:			return false;
		case ColliderType::OBB:				return false;
		case ColliderType::CIRCLE:			return CollisionCheck::CheckCollisionCircleCircle(std::make_shared<CircleCollider>(*this), std::dynamic_pointer_cast<CircleCollider>(collider));
		case ColliderType::CAPSULE:			return false;
		default:							return false;
		}
	}

	Collider::ColliderType CircleCollider::GetColliderType() const
	{
		return ColliderType::CIRCLE;
	}

	/////////////////////////////////////////////////////
	/////////////////// Line Collider ///////////////////
	/////////////////////////////////////////////////////
	LineCollider::LineCollider(glm::vec2 pointA, glm::vec2 pointB)
	{
		m_Point = pointA;
		m_Direction = glm::normalize(pointA - pointB);
	}

	bool LineCollider::IsColliding(const Ref<Collider>& collider)
	{
		return false;
	}

	Collider::ColliderType LineCollider::GetColliderType() const
	{
		return ColliderType::LINE;
	}

}