#pragma once

#include "SceneSerializer.h"
#include <pugixml.hpp>

namespace Apex {

	class XMLSceneSerializer : public SceneSerializer
	{
	public:
		XMLSceneSerializer(const Ref<Scene>& scene);
		~XMLSceneSerializer() = default;


	private:
		virtual void SerializeSceneHeader(std::stringstream& out) override;
		virtual void SerializeEntity(Entity& entity, std::stringstream& out) override;
		virtual void SerializeSceneFooter(std::stringstream& out) override;
		
		virtual bool DeserializeImpl(const std::string& buf) override;

	private:
		pugi::xml_document m_Document;
	};

}