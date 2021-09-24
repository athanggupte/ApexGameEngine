#include "apex_pch.h"
#include "ResourceSerializer.h"

#include "XMLResourceSerializer.h"

namespace Apex {

	ResourceSerializer::ResourceSerializer(ResourceManager& resourceManager)
		: m_ResourceManager(&resourceManager)
	{
	}

	void ResourceSerializer::SerializeResource(const Ref<File>& file, Handle handle)
	{
		SerializeResource(file, *m_ResourceManager->Get(handle));
	}

	void ResourceSerializer::SerializeResource(const Ref<File>& file, Resource& resource)
	{
		std::stringstream ss;
		SerializeHeader(ss);
		SerializeResource(resource, ss);
		SerializeFooter(ss);

		if (file && file->OpenWrite(false, false)) {
			auto buf = ss.str();
			file->Write(buf.c_str(), buf.size());
			file->Close();
		}
	}

	void ResourceSerializer::SerializeRegistry(const Ref<File>& file)
	{
		std::stringstream ss;
		SerializeHeader(ss);

		for (auto& [handle, resource] : m_ResourceManager->m_Registry) {
			SerializeResource(resource, ss);
		}

		SerializeFooter(ss);

		if (file && file->OpenWrite(false, false)) {
			auto buf = ss.str();
			file->Write(buf.c_str(), buf.size());
			file->Close();
		}
	}

	bool ResourceSerializer::Deserialize(const Ref<File>& file)
	{
		if (file && file->OpenRead()) {
			std::string buf(file->Size(), 0);
			file->Read(buf.data(), file->Size());
			file->Close();

			return DeserializeImpl(buf);
		}

		return false;
	}

	ResourceSerializerFactory& ResourceSerializerFactory::SetFormat(Format format)
	{
		m_Format = format;
		return *this;
	}

	Ref<ResourceSerializer> ResourceSerializerFactory::Build(ResourceManager& resourceManager)
	{
		switch (m_Format) {
		case Format::XML:
			return CreateRef<XMLResourceSerializer>(resourceManager);
		default:
			return nullptr;
		}
	}

}