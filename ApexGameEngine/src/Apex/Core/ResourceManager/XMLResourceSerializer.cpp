#include "apex_pch.h"
#include "XMLResourceSerializer.h"

#include "Apex/Utils/Utils.h"

namespace Apex {

	XMLResourceSerializer::XMLResourceSerializer(ResourceManager& resourceManager)
		: ResourceSerializer(resourceManager)
	{
	}

	//////// Serialize ////////

	void XMLResourceSerializer::SerializeHeader(std::stringstream& out)
	{
		m_Document.reset();
		m_Document.append_child(pugi::node_doctype).set_value("ApexResources");
		auto versionNode = m_Document.append_child(pugi::node_pi);
		versionNode.set_name("apex-version");
		versionNode.set_value("0.1");

		m_Document.append_child(pugi::node_comment).set_value("ApexGameEngine resource description file");

		m_Document.append_child("Resources");
	}

	void XMLResourceSerializer::SerializeFooter(std::stringstream& out)
	{
		m_Document.save(out, "  ");
	}

	void XMLResourceSerializer::SerializeResource(Resource& resource, std::stringstream& out)
	{
		auto resourceNode = m_Document.child("Resources").append_child("Resource");
		// resourceNode.append_attribute("id").set_value(BASE64(resource.GetId()).c_str());
		resourceNode.append_attribute("id").set_value(TO_CSTRING(Strings::Get(resource.GetId())));
		auto dataNode = resourceNode.append_child("Data");
		dataNode.append_attribute("Type").set_value((uint64_t)resource.GetType());

		pugi::xml_node typeNode;

		switch (resource.GetType()) {
		case ResourceType::FILE:
			typeNode = dataNode.append_child("File");
			// auto& resourceData = resource.Get<File>();
			typeNode.append_child("Source").append_child(pugi::node_pcdata).set_value(TO_CSTRING(resource.GetSource().str()));
			break;
		case ResourceType::TEXTURE:
			typeNode = dataNode.append_child("Texture");
			// auto& resourceData = resource.Get<Texture>();
			typeNode.append_child("Source").append_child(pugi::node_pcdata).set_value(TO_CSTRING(resource.GetSource().str()));
			break;
		case ResourceType::SHADER:
			typeNode = dataNode.append_child("Shader");
			// auto& resourceData = resource.Get<Shader>();
			typeNode.append_child("Source").append_child(pugi::node_pcdata).set_value(TO_CSTRING(resource.GetSource().str()));
			break;
		}
	}

	bool XMLResourceSerializer::DeserializeImpl(const std::string& buf)
	{
		auto result = m_Document.load_buffer_inplace((void*)buf.data(), buf.size(), pugi::parse_default | pugi::parse_doctype | pugi::parse_pi);
		if (!result)
			return false;

		for (auto child = m_Document.first_child(); child; child = child.next_sibling()) {
			// Check if file is an Apex Scene file
			if (child.type() == pugi::node_doctype && strcmp(child.value(), "ApexResources") != 0)
				return false;

			// Handle version (Currently only one version is supported)
			if (child.type() == pugi::node_pi
				&& strcmp(child.name(), "apex-version") == 0
				&& strcmp(child.value(), "0.1") != 0)
				return false;
		}

		auto resourcesNode = m_Document.child("Resources");
		if (!resourcesNode)
			return false;

		for (auto child = resourcesNode.first_child(); child; child = child.next_sibling()) {
			if (strcmp(child.name(), "Resource") != 0)
				continue;

			
		}

		return true;
	}

}