#include "apex_pch.h"
#include "Collidable.h"

namespace Apex {
	
	///////////// Collidable /////////////



	///////////// Collidables ////////////

	void Collidables::Init()
	{
		s_Instance = new Collidables();
	}

	void Collidables::Add(const Ref<Collidable>& item)
	{
		s_Instance->m_Collidables.push_back(item);
	}

	std::vector<Ref<Collidable>>::const_iterator Collidables::Begin()
	{
		return s_Instance->m_Collidables.begin();
	}
	
	std::vector<Ref<Collidable>>::const_iterator Collidables::End()
	{
		return s_Instance->m_Collidables.end();
	}

	void Collidables::Remove(const Ref<Collidable>& item)
	{
		auto it = std::find(s_Instance->m_Collidables.begin(), s_Instance->m_Collidables.end(), item);
		s_Instance->m_Collidables.erase(it);
	}

}