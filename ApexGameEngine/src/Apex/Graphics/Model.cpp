#include "apex_pch.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Apex/Core/FileSystem/FileSystem.h"
#include "Apex/Utils/Math.h"


namespace Apex {
	
	static bool s_LoadTextures = true;

	Ref<Model> Model::LoadModel(const std::string & path, bool loadTextures) {
		s_LoadTextures = loadTextures;
		return CreateRef<Model>(path);
	}

	Model::Model(const std::string & path)
	{
		std::string filepath;
		if (const auto file = FileSystem::GetFileIfExists(path))
			filepath = file->GetPhysicalPath().string();
		
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices |
														   aiProcess_GenNormals | aiProcess_FixInfacingNormals | aiProcess_CalcTangentSpace |
														   aiProcess_OptimizeMeshes | aiProcess_FindInvalidData |
												 0);

		APEX_CORE_ASSERT(!(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode), importer.GetErrorString());

		m_Directory = filepath.substr(0, filepath.find_last_of('/'));

		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode * node, const aiScene * scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			std::string meshId;
			if (mesh->mName.length)
				meshId += mesh->mName.C_Str() + std::to_string(m_Meshes.size());
			else
				meshId += "mesh-" + std::to_string(m_Meshes.size());
				
			m_Meshes[meshId] = CreateRef<Mesh>(ProcessMesh(mesh, scene));
			APEX_CORE_DEBUG("Mesh Loaded : {0}", meshId);
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene)
	{
		std::vector<float> vertices;
		
		APEX_CORE_DEBUG("UV Channels: {}", mesh->GetNumUVChannels());
		
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
			vertices.push_back(mesh->mNormals[i].x);
			vertices.push_back(mesh->mNormals[i].y);
			vertices.push_back(mesh->mNormals[i].z);
			
			if (mesh->HasTangentsAndBitangents()) {
				vertices.push_back(mesh->mTangents[i].x);
				vertices.push_back(mesh->mTangents[i].y);
				vertices.push_back(mesh->mTangents[i].z);
				
				vertices.push_back(mesh->mBitangents[i].x);
				vertices.push_back(mesh->mBitangents[i].y);
				vertices.push_back(mesh->mBitangents[i].z);
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

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		BufferLayout layout;
		if (mesh->HasTangentsAndBitangents()) {
			layout = {
				{ ShaderDataType::Float3, VertexElementType::Position },
				{ ShaderDataType::Float2, VertexElementType::UV0 },
				{ ShaderDataType::Float3, VertexElementType::Normal },
				{ ShaderDataType::Float3, VertexElementType::Tangent },
				{ ShaderDataType::Float3, VertexElementType::Bitangent },
			};
		} else {
			layout = {
				{ ShaderDataType::Float3, VertexElementType::Position },
				{ ShaderDataType::Float2, VertexElementType::UV0 },
				{ ShaderDataType::Float3, VertexElementType::Normal },
			};
		}
		Mesh _mesh(vertices.data(), vertices.size(), indices.data(), indices.size(), layout);
		//if(s_LoadTextures)
		//	_mesh.SetTextures(ProcessMaterial(material, aiTextureType_DIFFUSE, "TextureDiffuse"));
		
		_mesh.m_HasTangents = mesh->HasTangentsAndBitangents();
		
		return _mesh;
	}

	std::unordered_map<std::string, Ref<Texture>> Model::ProcessMaterial(aiMaterial * material, aiTextureType type, std::string typeName)
	{
		std::unordered_map<std::string, Ref<Texture>> textures;
		for (uint32_t i = 0; i < material->GetTextureCount(type); i++) {
			aiString str;
			material->GetTexture(type, i, &str);
			
			std::string filepath = m_Directory + "/" + std::string(str.C_Str());

			if (std::find(m_LoadedTexturePaths.begin(), m_LoadedTexturePaths.end(), filepath) == m_LoadedTexturePaths.end()) {
				Ref<Texture2D> texture = Texture2D::Create(filepath);
				textures[typeName] = texture;
				m_LoadedTexturePaths.push_back(filepath);
			}
		}

		return textures;
	}

}
