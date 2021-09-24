#pragma once

#include "ResourceSerializer.h"
#include "Apex/Core/FileSystem/FileSystem.h"

#include <pugixml.hpp>

namespace Apex {

	class XMLResourceSerializer : public ResourceSerializer
	{
	public:
		XMLResourceSerializer(ResourceManager& resourceManager);
		virtual ~XMLResourceSerializer() = default;

	protected:
		virtual void SerializeHeader(std::stringstream& out) override;
		virtual void SerializeFooter(std::stringstream& out) override;
		virtual void SerializeResource(Resource& resource, std::stringstream& out) override;

		virtual bool DeserializeImpl(const std::string& buf) override;

	private:
		pugi::xml_document m_Document;
	};

}