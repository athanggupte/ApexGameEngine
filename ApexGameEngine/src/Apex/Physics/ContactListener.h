#pragma once

namespace Apex {
	class Entity;

	class IContactListener
	{
	public:
		virtual ~IContactListener() = default;

		virtual void OnContact(Entity other_entity) = 0;
		virtual void OnOverlapBegin(Entity other_entity) = 0;
		virtual void OnOverlapEnd(Entity other_entity) = 0;
	};

}
