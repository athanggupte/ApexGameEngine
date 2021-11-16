#include "apex_pch.h"
#include "Mesh.h"

#include "Apex/Core/FileSystem/FileSystem.h"
#include "Apex/Graphics/RenderPrimitives/Buffer.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Apex {

	static void ProcessMesh(aiMesh * mesh, const aiScene * scene, std::vector<float>& vertices, std::vector<uint32_t>& indices, const BufferLayout& layout, uint32_t& vertexCount)
	{
		const uint32_t prevVertexCount = vertexCount;
		APEX_CORE_TRACE(__FUNCTION__" :: Vertex start index : {}", prevVertexCount);

		for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
			vertices.push_back(mesh->mVertices[i].x);
			vertices.push_back(mesh->mVertices[i].y);
			vertices.push_back(mesh->mVertices[i].z);
			if (layout.HasVertexElementType(VertexElementType::TextureCoords)) {
				if (mesh->HasTextureCoords(0)) {
					vertices.push_back(mesh->mTextureCoords[0][i].x);
					vertices.push_back(mesh->mTextureCoords[0][i].y);
				} else {
					vertices.push_back(0.f);
					vertices.push_back(0.f);
				}
			}
			++vertexCount;
		}

		for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (uint32_t f = 0; f < face.mNumIndices; f++) {
				indices.push_back(face.mIndices[f] + prevVertexCount);
			}
		}
	}

	static void ProcessNode(aiNode* node, const aiScene* scene, std::vector<float>& vertices, std::vector<uint32_t>& indices, const BufferLayout& layout, uint32_t& vertexCount, int level = 0)
	{
		APEX_CORE_TRACE("Processing node @ level: {}", level);
		APEX_CORE_TRACE(__FUNCTION__" :: Vertex start index : {}", vertexCount);

		for (uint32_t i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			ProcessMesh(mesh, scene, vertices, indices, layout, vertexCount);
			break;
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene, vertices, indices, layout, vertexCount, level + 1);
		}
	}

	Mesh::Mesh(const std::string& path)
	{
		std::string filepath;
		if (const auto file = FileSystem::GetFileIfExists(path))
			filepath = file->GetPhysicalPath().string();

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices |
														   aiProcess_GenNormals | aiProcess_FixInfacingNormals | aiProcess_CalcTangentSpace |
														   aiProcess_OptimizeMeshes | aiProcess_ValidateDataStructure |
												 0);

		APEX_CORE_ASSERT(!(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode), importer.GetErrorString());

		std::vector<float> vertices;
		std::vector<uint32_t> indices;

		const BufferLayout layout = {
				{ ShaderDataType::Float3, VertexElementType::Position },
				{ ShaderDataType::Float2, VertexElementType::TextureCoords },
			};

		uint32_t vertexCount = 0;
		ProcessNode(scene->mRootNode, scene, vertices, indices, layout, vertexCount, 0);

		APEX_CORE_INFO("Vertex Count : {}", vertexCount);

		if (indices.empty())
			*this = Mesh(vertices.data(), vertices.size(), layout);
		else
			*this = Mesh(vertices.data(), vertices.size(), indices.data(), indices.size(), layout);
	}

	Mesh::Mesh(float* vertices, size_t vCount, uint32_t* indices, size_t iCount, const BufferLayout& layout)
	{
		m_VAO = VertexArray::Create();
		Ref<VertexBuffer> vbo = VertexBuffer::Create(vertices, vCount * sizeof(float));
		Ref<IndexBuffer> ibo = IndexBuffer::Create(indices, iCount);
		
		vbo->SetLayout(layout);
		
		m_VAO->AddVertexBuffer(vbo);
		m_VAO->AddIndexBuffer(ibo);
		
		m_VAO->Unbind();
	}

	Mesh::Mesh(float* vertices, size_t vCount, const BufferLayout& layout)
	{
		m_VAO = VertexArray::Create();
		Ref<VertexBuffer> vbo = VertexBuffer::Create(vertices, vCount * sizeof(float));
		
		vbo->SetLayout(layout);
		
		m_VAO->AddVertexBuffer(vbo);
		
		m_VAO->Unbind();
	}

	//void Mesh::AddTexture(std::pair<std::string, Ref<Texture>>& nameTexturePair)
	//{
	//	m_Textures.insert(nameTexturePair);
	//}

	//void Mesh::AddTexture(std::string name, Ref<Texture> texture)
	//{
	//	m_Textures[name] = texture;
	//}


}
