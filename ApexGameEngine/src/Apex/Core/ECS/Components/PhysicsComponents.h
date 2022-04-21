#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#define COMPONENT_DEFAULT_CTR(name)	name() = default;\
									name(const name&) = default

#define REGISTER_COMPONENT(name) static constexpr const char* TypeName = STR(name);\
								COMPONENT_DEFAULT_CTR(name)

namespace Apex {

	enum class RigidBodyType
	{
		Static,
		Kinematic,
		Dynamic,

		_COUNT
	};

	constexpr const char* RigidBodyTypeString(RigidBodyType type)
	{
		switch (type)
		{
		case RigidBodyType::Static: return "Static";
		case RigidBodyType::Kinematic: return "Kinematic";
		case RigidBodyType::Dynamic: return "Dynamic";
		default: return "UNKNOWN";
		}
	}

	struct RigidBodyComponent
	{
		float density = 1.f;
		RigidBodyType type = RigidBodyType::Static;
		bool useGravity = true;
	};

	struct SphereCollider
	{
		float radius = 1.f;

		COMPONENT_DEFAULT_CTR(SphereCollider);

		SphereCollider(float radius)
			: radius(radius) {}
	};

	struct BoxCollider
	{
		glm::vec3 halfExtents { 0.5f };

		//// Array Element
		//uint16_t arraySize = 0; // Num of elements next
		//entt::entity parent = entt::null;
		//entt::entity next = entt::null;
		//entt::entity prev = entt::null;

		COMPONENT_DEFAULT_CTR(BoxCollider);

		BoxCollider(float half_x, float half_y, float half_z)
			: halfExtents(half_x, half_y, half_z) {}

		BoxCollider(const glm::vec3& half_extends)
			: halfExtents(half_extends) {}


	};

	struct __ComponentArrayElement
	{
		uint32_t arraySize = 0; // Num of elements next
		entt::entity parent = entt::null;
		entt::entity next = entt::null;
		entt::entity prev = entt::null;
	};

	struct BoxColliderArrayElement
	{
		BoxCollider collider;
		__ComponentArrayElement arrayElement;

		BoxCollider& AddNext(Entity entity)
		{
			Entity newEntity = entity.GetScene()->CreateEntity(HASH("__A_BoxCollider"));
			arrayElement.next = newEntity;
			
			auto& bcArrElem = newEntity.AddComponent<BoxColliderArrayElement>();
			bcArrElem.arrayElement.prev = entity;

			return bcArrElem.collider;
		}
	};

	struct ContactListenerIdComponent
	{
		size_t listenerId;
	};

}