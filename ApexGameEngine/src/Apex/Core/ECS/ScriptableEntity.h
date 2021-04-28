#pragma once
#include "Entity.h"

namespace Apex {
	
	class ScriptableEntity
	{
	public:
		ScriptableEntity() = default;
		ScriptableEntity(const ScriptableEntity&) = default;
		ScriptableEntity(Entity entity, Scene* scene) : m_Entity(entity), m_Scene(scene) {}
		
		template<typename Component_t>
		inline decltype(auto) GetComponent()
		{
			return m_Entity.GetComponent<Component_t>();
		}
		
	private:
		Entity m_Entity;
		Scene* m_Scene;
		
		friend class Scene;
	};
	
}
