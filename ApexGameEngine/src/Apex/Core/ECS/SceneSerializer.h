#pragma once

#include "Scene.h"
#include "Apex/Core/ECS/Components.h"

namespace Apex {

	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);
		virtual ~SceneSerializer() = default;

		void Serialize(const fs::path& filepath);
		bool Deserialize(const fs::path& filepath);

	protected:
		virtual void SerializeSceneHeader(std::stringstream& out) = 0;
		virtual void SerializeEntity(Entity& entity, std::stringstream& out) = 0;
		virtual void SerializeSceneFooter(std::stringstream& out) = 0;

		virtual bool DeserializeImpl(const std::string& buf) = 0;

	protected:
		Ref<Scene> m_Scene;
	};

	class SceneSerializerFactory
	{
	public:
		enum class Format
		{
			XML
		};

	public:
		SceneSerializerFactory() = default;

		SceneSerializerFactory& SetFormat(Format format);
		Ref<SceneSerializer> Build(const Ref<Scene>& scene);

	private:
		Format m_Format;
	};

}