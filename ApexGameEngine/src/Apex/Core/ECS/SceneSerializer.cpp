#include "apex_pch.h"
#include "SceneSerializer.h"

#include "XMLSceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include "Apex/Core/FileSystem/VFS.h"

#include <fstream>
#include <pugixml.hpp>

namespace Apex {
    
    SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
        : m_Scene(scene)
    {
    }

    void SceneSerializer::Serialize(const std::string& filepath)
    {
        std::stringstream ss;

        SerializeSceneHeader(ss);

        m_Scene->m_Registry.each([&](auto entityId) {
            Entity entity = { entityId, m_Scene.get() };
            if (entity)
                SerializeEntity(entity, ss);
        });
        
        SerializeSceneFooter(ss);

        auto file = FileSystem::MakeFile(filepath);
        if (file && file->OpenWrite(false, false)) {
            auto buf = ss.str();
            file->Write(buf.c_str(), buf.size());
            file->Close();
        }
    }

    bool SceneSerializer::Deserialize(const std::string& filepath)
    {
        auto file = FileSystem::GetFileIfExists(filepath);
        if (file && file->OpenRead()) {
            std::string buf(file->Size(), 0);
            file->Read(buf.data(), file->Size());
            file->Close();

            return DeserializeImpl(buf);
        }

        return false;
    }

    // Scene Serializer Factory
    
    SceneSerializerFactory& SceneSerializerFactory::SetFormat(Format format)
    {
        m_Format = format;
        return *this;
    }

    Ref<SceneSerializer> SceneSerializerFactory::Build(const Ref<Scene>& scene)
    {
        switch (m_Format) {
        case Format::XML:
            return CreateRef<XMLSceneSerializer>(scene);
        default:
            return nullptr;
        }
    }
}