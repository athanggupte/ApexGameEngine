#include "apex_pch.h"
#include "XMLSceneSerializer.h"

#include "Apex/Application.h"
#include "Apex/Utils/Utils.h"

namespace Apex {

    XMLSceneSerializer::XMLSceneSerializer(const Ref<Scene>& scene)
        : SceneSerializer(scene)
    {
    }

    //////// Serialize ////////

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
        //node.append_attribute("x").set_value(vec.x);
        //node.append_attribute("y").set_value(vec.y);
        //node.append_attribute("z").set_value(vec.z);

        node.append_child("x").append_child(pugi::node_pcdata).set_value(TO_CSTRING(vec.x));
        node.append_child("y").append_child(pugi::node_pcdata).set_value(TO_CSTRING(vec.y));
        node.append_child("z").append_child(pugi::node_pcdata).set_value(TO_CSTRING(vec.z));
    }

    void SerializeVec(pugi::xml_node& node, const glm::vec4& vec)
    {
        node.append_child("x").append_child(pugi::node_pcdata).set_value(TO_CSTRING(vec.x));
        node.append_child("y").append_child(pugi::node_pcdata).set_value(TO_CSTRING(vec.y));
        node.append_child("z").append_child(pugi::node_pcdata).set_value(TO_CSTRING(vec.z));
        node.append_child("w").append_child(pugi::node_pcdata).set_value(TO_CSTRING(vec.w));
    }

    void XMLSceneSerializer::SerializeEntity(Entity& entity, std::stringstream& out)
    {
        auto entityNode = m_Document.child("Scene").child("Entities").append_child("Entity");
        entityNode.append_attribute("id").set_value(BASE64(1297814978).c_str());

        if (entity.HasComponent<TagComponent>()) {
            const auto& tag = entity.GetComponent<TagComponent>().tag;
            auto tagNode = entityNode.append_child("TagComponent");
            tagNode.append_child(pugi::node_pcdata).set_value(std::string(tag.str()).c_str());
        }

        if (entity.HasComponent<TransformComponent>()) {
            const auto& transform = entity.GetComponent<TransformComponent>();
            auto transformNode = entityNode.append_child("TransformComponent");

            auto translationNode = transformNode.append_child("Translation");
            SerializeVec(translationNode, transform.translation);
            auto rotationNode = transformNode.append_child("Rotation");
            SerializeVec(rotationNode, transform.rotation);
            auto scaleNode = transformNode.append_child("Scale");
            SerializeVec(scaleNode, transform.scale);
        }

        /*if (entity.HasComponent<CameraComponent>()) {
            const auto& camera = entity.GetComponent<CameraComponent>().Camera;
            auto cameraNode = entityNode.append_child("CameraComponent");
            camera.
        }*/

        if (entity.HasComponent<SpriteRendererComponent>()) {
            const auto& sprite = entity.GetComponent<SpriteRendererComponent>();
            auto spriteNode = entityNode.append_child("SpriteRendererComponent");

            if (sprite.texture.IsValid()) {
                auto textureNode = spriteNode.append_child("Texture");
                textureNode.append_child("ResourceId").append_child(pugi::node_pcdata).set_value(TO_CSTRING(Strings::Get(sprite.texture.GetId())));
                textureNode.append_child("TilingFactor").append_child(pugi::node_pcdata).set_value(TO_CSTRING(sprite.tilingFactor));
                textureNode.append_attribute("use").set_value(sprite.useTexture);
            }

            auto colorNode = spriteNode.append_child("Color");
            SerializeVec(colorNode, sprite.color);
        }

        if (entity.HasComponent<MeshRendererComponent>()) {
            const auto& meshComp = entity.GetComponent<MeshRendererComponent>();
            auto meshCompNode = entityNode.append_child("MeshRendererComponent");

        	auto meshNode = meshCompNode.append_child("Mesh");
            if (meshComp.mesh.IsValid()) {
                meshNode.append_child("ResourceId").append_child(pugi::node_pcdata).set_value(TO_CSTRING(Strings::Get(meshComp.mesh.GetId())));
            } else {
	            meshNode.append_child("ResourceId").append_child(pugi::node_pcdata).set_value("");
            }

            auto materialNode = meshCompNode.append_child("Material");
            if (meshComp.material.IsValid()) {
                materialNode.append_child("ResourceId").append_child(pugi::node_pcdata).set_value(TO_CSTRING(Strings::Get(meshComp.material.GetId())));
            } else {
	            meshNode.append_child("ResourceId").append_child(pugi::node_pcdata).set_value("");
            }

        }

    }

    void XMLSceneSerializer::SerializeSceneFooter(std::stringstream& out)
    {
        m_Document.save(out, "  ");
    }


    //////// Deserialize ////////

    void DeserializeVec(const pugi::xml_node& node, glm::vec3& vec)
    {
        vec.x = std::stof(node.child_value("x"));
        vec.y = std::stof(node.child_value("y"));
        vec.z = std::stof(node.child_value("z"));
    }

    void DeserializeVec(const pugi::xml_node& node, glm::vec4& vec)
    {
        vec.x = std::stof(node.child_value("x"));
        vec.y = std::stof(node.child_value("y"));
        vec.z = std::stof(node.child_value("z"));
        vec.w = std::stof(node.child_value("w"));
    }

    bool DeserializeEntity(pugi::xml_node& node, Entity& entity)
    {
	    const auto tagNode = node.child("TagComponent");
        if (!tagNode)
            return false;
        entity.GetComponent<TagComponent>().tag = HASH(tagNode.child_value());

        APEX_CORE_DEBUG("Importing entity '{}'", entity.GetComponent<TagComponent>().tag.str());

	    const auto transformNode = node.child("TransformComponent");
        if (!transformNode)
            return false;

        auto& transform = entity.GetComponent<TransformComponent>();
        const auto translationNode = transformNode.child("Translation");
        DeserializeVec(translationNode, transform.translation);
	    const auto rotationNode = transformNode.child("Rotation");
        DeserializeVec(rotationNode, transform.rotation);
	    const auto scaleNode = transformNode.child("Scale");
        DeserializeVec(scaleNode, transform.scale);

        if (const auto spriteNode = node.child("SpriteRendererComponent")) {
            auto& sprite = entity.AddComponent<SpriteRendererComponent>();
            if (const auto textureNode = spriteNode.child("Texture")) {
                sprite.texture = Application::Get().GetResourceManager().Get<Texture>(RESNAME(textureNode.child_value("ResourceId")));
                sprite.tilingFactor = std::stof(textureNode.child_value("TilingFactor"));
                sprite.useTexture = textureNode.attribute("use").as_bool();
            }
            const auto colorNode = spriteNode.child("Color");
            DeserializeVec(colorNode, sprite.color);
        }

        if (const auto meshCompNode = node.child("MeshRendererComponent")) {
            auto& meshComp = entity.AddComponent<MeshRendererComponent>();
            if (const auto meshNode = meshCompNode.child("Mesh")) {
                meshComp.mesh = Application::Get().GetResourceManager().Get<Mesh>(RESNAME(meshNode.child_value("ResourceId")));
            }
            if (const auto materialNode = meshCompNode.child("Material")) {
                meshComp.material = Application::Get().GetResourceManager().Get<Material>(RESNAME(materialNode.child_value("ResourceId")));
            }
        }

        // Other Components

        return true;
    }

    bool XMLSceneSerializer::DeserializeImpl(const std::string& buf)
    {
	    const auto result = m_Document.load_buffer_inplace((void*)buf.data(), buf.size(), pugi::parse_default | pugi::parse_doctype | pugi::parse_pi);
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

	    const auto sceneNode = m_Document.child("Scene");
        if (!sceneNode)
            return false;

    	const auto entitiesNode = sceneNode.child("Entities");
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

}