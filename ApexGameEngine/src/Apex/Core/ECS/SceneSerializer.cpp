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
        auto file = FileSystem::MakeFile(filepath);
        if (file && file->OpenRead()) {
            std::string buf(file->Size(), 0);
            file->Read(buf.data(), file->Size());
            file->Close();

            return DeserializeImpl(buf);
        }

        return false;
    }

    XMLSceneSerializer::XMLSceneSerializer(const Ref<Scene>& scene)
        : SceneSerializer(scene)
    {
    }

    void XMLSceneSerializer::SerializeSceneHeader(std::stringstream& out)
    {
        m_Document.reset();
        m_Document.append_child(pugi::node_doctype).set_value("ApexScene");
        auto versionNode = m_Document.append_child(pugi::node_pi);
        versionNode.set_name("apex-version");
        versionNode.set_value("0.1");

        m_Document.append_child(pugi::node_comment).set_value("ApexGameEngine scene description file");

        auto sceneNode = m_Document.append_child("Scene");
        sceneNode.append_attribute("name") = "Untitled";
        sceneNode.append_child("Entities");
    }

    void SerializeVec(pugi::xml_node& node, const glm::vec3& vec)
    {
        node.append_attribute("x").set_value(vec.x);
        node.append_attribute("y").set_value(vec.y);
        node.append_attribute("z").set_value(vec.z);
        // node.append_child("x").append_child(pugi::node_pcdata).set_value(std::to_string(vec.x).c_str());
        // node.append_child("y").append_child(pugi::node_pcdata).set_value(std::to_string(vec.y).c_str());
        // node.append_child("z").append_child(pugi::node_pcdata).set_value(std::to_string(vec.z).c_str());
    }

    void SerializeVec(pugi::xml_node& node, const glm::vec4& vec)
    {
        node.append_child("x").append_child(pugi::node_pcdata).set_value(std::to_string(vec.x).c_str());
        node.append_child("y").append_child(pugi::node_pcdata).set_value(std::to_string(vec.y).c_str());
        node.append_child("z").append_child(pugi::node_pcdata).set_value(std::to_string(vec.z).c_str());
        node.append_child("w").append_child(pugi::node_pcdata).set_value(std::to_string(vec.w).c_str());
    }

    void XMLSceneSerializer::SerializeEntity(Entity& entity, std::stringstream& out)
    {
        auto entityNode = m_Document.child("Scene").child("Entities").append_child("Entity");
        entityNode.append_attribute("id") = 1297814978;
        
        if (entity.HasComponent<TagComponent>()) {
            auto& tag = entity.GetComponent<TagComponent>().Tag;
            auto tagNode = entityNode.append_child("TagComponent");
            tagNode.append_child(pugi::node_pcdata).set_value(tag.c_str());
        }

        if (entity.HasComponent<TransformComponent>()) {
            const auto& transform = entity.GetComponent<TransformComponent>();
            auto transformNode = entityNode.append_child("TransformComponent");

            auto translationNode = transformNode.append_child("Translation");
            SerializeVec(translationNode, transform.Translation);
            auto rotationNode = transformNode.append_child("Rotation");
            SerializeVec(rotationNode, transform.Rotation);
            auto scaleNode = transformNode.append_child("Scale");
            SerializeVec(scaleNode, transform.Scale);
        }

        /*
        APEX_CORE_TRACE("Entity: {}", entity.GetComponent<TagComponent>().Tag);
        entity.Visit([](const entt::type_info ti) {
            APEX_CORE_TRACE("{}", ti.name());
            if (ti == entt::type_id<TransformComponent>()) {
            }
            if (ti == entt::type_id<CameraComponent>()) {
            }
        });
        */
    }

    void XMLSceneSerializer::SerializeSceneFooter(std::stringstream& out)
    {
        m_Document.save(out, "  ");
    }


    bool DeserializeEntity(pugi::xml_node& node, Entity& entity)
    {
        auto tagNode = node.child("TagComponent");
        if (!tagNode)
            return false;
        entity.GetComponent<TagComponent>().Tag = tagNode.child_value();

        APEX_CORE_DEBUG("Importing entity '{}'", entity.GetComponent<TagComponent>().Tag);

        auto transformNode = node.child("TransformComponent");
        if (!transformNode)
            return false;

        auto& translation = entity.GetComponent<TransformComponent>().Translation;
        auto translationNode = transformNode.child("Translation");
        translation.x = translationNode.attribute("x").as_float();
        translation.y = translationNode.attribute("y").as_float();
        translation.z = translationNode.attribute("z").as_float();

        auto& rotation = entity.GetComponent<TransformComponent>().Translation;
        auto rotationNode = transformNode.child("Rotation");
        rotation.x = rotationNode.attribute("x").as_float();
        rotation.y = rotationNode.attribute("y").as_float();
        rotation.z = rotationNode.attribute("z").as_float();

        auto& scale = entity.GetComponent<TransformComponent>().Scale;
        auto scaleNode = transformNode.child("Scale");
        scale.x = scaleNode.attribute("x").as_float();
        scale.y = scaleNode.attribute("y").as_float();
        scale.z = scaleNode.attribute("z").as_float();

        // Other Components
        
        return true;
    }

    bool XMLSceneSerializer::DeserializeImpl(const std::string& buf)
    {
        auto result = m_Document.load_buffer_inplace((void*)buf.data(), buf.size(), pugi::parse_default | pugi::parse_doctype | pugi::parse_pi);
        if (!result)
            return false;

        for (auto child = m_Document.first_child(); child; child = child.next_sibling()) {
            // Check if file is an Apex Scene file
            if (child.type() == pugi::node_doctype && strcmp(child.value(), "ApexScene") != 0)
                return false;

            // Handle version (Currently only one version is supported)
            if (child.type() == pugi::node_pi
                && strcmp(child.name(), "apex-version") == 0
                && strcmp(child.value(), "0.1") != 0)
                return false;
        }

        auto sceneNode = m_Document.child("Scene");
        if (!sceneNode)
            return false;

        auto entitiesNode = sceneNode.child("Entities");
        if (!entitiesNode)
            return true;

        for (auto child = entitiesNode.first_child(); child; child = child.next_sibling()) {
            if (strcmp(child.name(), "Entity") != 0)
                continue;

            if (!DeserializeEntity(child, m_Scene->CreateEntity()))
                return false;
        }

        return true;
    }
    
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