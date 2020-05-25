#pragma once

#include "Collider.h"

namespace Apex {

	class Collidable
	{
	public:
		Collidable(const Ref<Collider>& collider)
			: m_Collider(collider)
		{
		}
		virtual ~Collidable() = default;

		inline void SetCollider(const Ref<Collider>& collider) { this->m_Collider = collider; }
		const Ref<Collider>& GetCollider() { return m_Collider; }
	protected:
		Ref<Collider> m_Collider;
	};

	class Collidables
	{
	public:
		static void Init();

		static void Add(const Ref<Collidable>& item);
		//static const Ref<Collidable>& Get(int i);
		static std::vector<Ref<Collidable>>::const_iterator Begin();
		static std::vector<Ref<Collidable>>::const_iterator End();
		static void Remove(const Ref<Collidable>& item);

	private:
		static Collidables* s_Instance;
		std::vector<Ref<Collidable>> m_Collidables;
	};
}