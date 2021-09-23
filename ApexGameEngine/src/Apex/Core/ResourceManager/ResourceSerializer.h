#pragma once

#include "ResourceManager.h"
#include "Apex/Core/FileSystem/VFS.h"

namespace Apex {

	class ResourceSerializer
	{
	public:
		ResourceSerializer(ResourceManager& resourceManager);
		virtual ~ResourceSerializer() = default;

		void SerializeResource(const Ref<File>& file, Handle handle);
		void SerializeResource(const Ref<File>& file, Resource& resource);

		void SerializeRegistry(const Ref<File>& file);
		bool Deserialize(const Ref<File>& file);

	protected:
		virtual void SerializeHeader(std::stringstream& out) = 0;
		virtual void SerializeFooter(std::stringstream& out) = 0;
		virtual void SerializeResource(Resource& resource, std::stringstream& out) = 0;

		virtual bool DeserializeImpl(const std::string& buf) = 0;

	protected:
		ResourceManager* m_ResourceManager;
		
	};

	class ResourceSerializerFactory
	{
	public:
		enum class Format
		{
			XML
		};

	public:
		ResourceSerializerFactory() = default;

		ResourceSerializerFactory& SetFormat(Format format);
		Ref<ResourceSerializer> Build(ResourceManager& resourceManager);

	private:
		Format m_Format;
	};

}