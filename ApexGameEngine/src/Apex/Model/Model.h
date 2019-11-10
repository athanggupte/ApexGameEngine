#pragma once

#include "assimp/scene.h"
#include "Mesh.h"

namespace Apex {

	class Model
	{
	public:
		Model(const std::string& path);
		virtual ~Model() = default;

		virtual inline const std::vector<Mesh>& GetMeshes() { return this->m_Meshes; }

	private:
		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	private:
		std::vector<Mesh> m_Meshes;
	};

}