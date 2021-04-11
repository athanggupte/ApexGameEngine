#pragma once

#include "assimp/scene.h"
#include "Mesh.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"

namespace Apex {

	class Model
	{
	public:
		Model(const std::string& path);
		virtual ~Model() = default;

		virtual inline const std::unordered_map<std::string, Ref<Mesh>>& GetMeshes() const { return this->m_Meshes; }
		virtual inline const std::vector<std::string>& GetLoadedTexturePaths() const { return this->m_LoadedTexturePaths; }

		virtual inline const glm::mat4& GetModelMatrix() const { return m_ModelMatrix; }
		virtual inline void SetModelMatrix(const glm::mat4& modelMatrix) const { m_ModelMatrix = modelMatrix; }
		virtual inline void ApplyModelMatrix(const glm::mat4& modelMatrix) const { m_ModelMatrix = modelMatrix * m_ModelMatrix; }

		static Ref<Model> LoadModel(const std::string& path, bool loadTextures = true);

	private:
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		std::unordered_map<std::string, Ref<Texture>> ProcessMaterial(aiMaterial* material, aiTextureType type, std::string typeName);

	private:
		std::string m_Directory;
		std::unordered_map<std::string, Ref<Mesh>> m_Meshes;
		std::vector<std::string> m_LoadedTexturePaths;
		mutable glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
	};

}
