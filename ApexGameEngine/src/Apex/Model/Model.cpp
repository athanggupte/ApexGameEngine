#include "apex_pch.h"
#include "Model.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "Apex/MathPrimitiveParser.h"

namespace Apex {

	Model::Model(const std::string & path)
	{
		LoadModel(path);
	}

	void Model::LoadModel(const std::string & path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs);

		APEX_CORE_ASSERT(!(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode), importer.GetErrorString());

		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode * node, const aiScene * scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_Meshes.push_back(ProcessMesh(mesh, scene));
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene)
	{
		std::vector<float> vertices;

		for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
			vertices.push_back(mesh->mVertices[i].x);
			vertices.push_back(mesh->mVertices[i].y);
			vertices.push_back(mesh->mVertices[i].z);
			if (mesh->mTextureCoords[0]) {
				vertices.push_back(mesh->mTextureCoords[0][i].x);
				vertices.push_back(mesh->mTextureCoords[0][i].y);
			}
			else {
				vertices.push_back(0.0f);
				vertices.push_back(0.0f);
			}
		}

		/*APEX_CORE_TRACE("Vertices ->");
		for (uint32_t i = 0; i < vertices.size(); i += 5) {
			APEX_CORE_TRACE("{0},{1},{2},{3},{4}", vertices.at(i), vertices.at(i + 1), vertices.at(i + 2), vertices.at(i + 3), vertices.at(i + 4));
		}*/

		std::vector<uint32_t> indices;

		for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (uint32_t f = 0; f < face.mNumIndices; f++) {
				indices.push_back(face.mIndices[f]);
			}
		}

		/*APEX_CORE_TRACE("Indices ->");
		for (uint32_t i = 0; i < indices.size(); i += 3) {
			APEX_CORE_TRACE("{0},{1},{2}", indices.at(i), indices.at(i + 1), indices.at(i + 2));
		}*/

		return Mesh(vertices.data(), vertices.size(), indices.data(), indices.size());
	}

}