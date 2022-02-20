#include "apex_pch.h"
#include "Mesh.h"

#include "Apex/Core/FileSystem/FileSystem.h"
#include "Apex/Graphics/RenderPrimitives/Buffer.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Apex {

#ifdef APEX_USE_ASSIMP

	static void ProcessMesh(aiMesh * mesh, const aiScene * scene, std::vector<float>& vertices, std::vector<uint32_t>& indices, const BufferLayout& layout, uint32_t& vertexCount)
	{
		const uint32_t prevVertexCount = vertexCount;
		APEX_CORE_TRACE(__FUNCTION__" :: Vertex start index : {}", prevVertexCount);

		for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
			vertices.push_back(mesh->mVertices[i].x);
			vertices.push_back(mesh->mVertices[i].y);
			vertices.push_back(mesh->mVertices[i].z);
			if (layout.HasVertexElementType(VertexElementType::UV0)) {
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

	Mesh::Mesh(const fs::path& path)
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
				{ ShaderDataType::Float2, VertexElementType::UV0 },
			};

		uint32_t vertexCount = 0;
		ProcessNode(scene->mRootNode, scene, vertices, indices, layout, vertexCount, 0);

		APEX_CORE_INFO("Vertex Count : {}", vertexCount);

		if (indices.empty())
			*this = Mesh(vertices.data(), vertices.size(), layout);
		else
			*this = Mesh(vertices.data(), vertices.size(), indices.data(), indices.size(), layout);
	}

#endif


	Mesh::Mesh(float* vertices, size_t vCount, uint32_t* indices, size_t iCount, const BufferLayout& layout)
		: m_HasNormals(layout.HasVertexElementType(VertexElementType::Normal)),
		  m_HasTangents(layout.HasVertexElementType(VertexElementType::Tangent))
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
		: m_HasNormals(layout.HasVertexElementType(VertexElementType::Normal)),
		  m_HasTangents(layout.HasVertexElementType(VertexElementType::Tangent))
	{
		m_VAO = VertexArray::Create();
		Ref<VertexBuffer> vbo = VertexBuffer::Create(vertices, vCount * sizeof(float));
		
		vbo->SetLayout(layout);
		
		m_VAO->AddVertexBuffer(vbo);
		
		m_VAO->Unbind();
	}

	// Mesh Builder

	static std::vector<glm::vec3> s_Normals;
	static std::vector<glm::vec4> s_Tangents;

	MeshBuilder::MeshBuilder()
		: m_Indices(nullptr), m_Positions(nullptr), m_UVs(nullptr), m_Normals(nullptr),
	      m_Tangents(nullptr), m_Bitangents(nullptr), m_TexIndexes(nullptr)
	{
	}

	void MeshBuilder::RegenerateSoftNormals()
	{
		APEX_CORE_ASSERT(m_Positions, "Positions is NULL! Cannot generate normal data without positions.");
		APEX_CORE_ASSERT(m_Indices, "Indices is NULL! Cannot generate normal data without indices.");

		if (!m_Normals) {
			s_Normals.resize(m_Positions->size(), glm::vec3{ 0.f });
			m_Normals = &s_Normals;
		}
		
		for (int i = 0; i < m_Indices->size(); i += 3) {
			auto i0 = (*m_Indices)[i + 0];
			auto i1 = (*m_Indices)[i + 1];
			auto i2 = (*m_Indices)[i + 2];

			auto a = (*m_Positions)[i0];
			auto b = (*m_Positions)[i1];
			auto c = (*m_Positions)[i2];

			auto e1 = b - a;
			auto e2 = c - a;

			auto n = glm::cross(e1, e2);

			(*m_Normals)[i0] += n;
			(*m_Normals)[i1] += n;
			(*m_Normals)[i2] += n;
		}

		for (auto& normal : *m_Normals) {
			normal = glm::normalize(normal);
		}
	}

	void MeshBuilder::RegenerateFlatNormals()
	{
		APEX_CORE_ASSERT(m_Positions, "Positions is NULL! Cannot generate normal data without positions.");
		if (!m_Normals) {
			s_Normals.resize(m_Positions->size());
			m_Normals = &s_Normals;
		}

		if (m_Indices) {
			for (int i = 0; i < m_Indices->size(); i += 3) {
				auto i0 = (*m_Indices)[i + 0];
				auto i1 = (*m_Indices)[i + 1];
				auto i2 = (*m_Indices)[i + 2];

				auto a = (*m_Positions)[i0];
				auto b = (*m_Positions)[i1];
				auto c = (*m_Positions)[i2];



				auto e1 = b - a;
				auto e2 = c - a;

				auto n = glm::normalize(glm::cross(e1, e2));

				(*m_Normals)[i0] = n;
				(*m_Normals)[i1] = n;
				(*m_Normals)[i2] = n;
			}
		} else {
			for (int i = 0; i < m_Positions->size(); i += 3) {
				auto a = (*m_Positions)[i + 0];
				auto b = (*m_Positions)[i + 1];
				auto c = (*m_Positions)[i + 2];

				auto e1 = b - a;
				auto e2 = c - a;

				auto n = glm::normalize(glm::cross(e1, e2));

				(*m_Normals)[i + 0] = n;
				(*m_Normals)[i + 1] = n;
				(*m_Normals)[i + 2] = n;
			}
		}
	}

	void MeshBuilder::RegenerateTangents()
	{
		APEX_CORE_ASSERT(m_Positions, "Positions is NULL! Cannot generate normal data without positions.");
		APEX_CORE_ASSERT(m_Normals, "Indices is NULL! Cannot generate normal data without normals.");
		if (!m_Tangents) {
			s_Tangents.resize(m_Positions->size(), glm::vec4{ 1.f, 0.f, 0.f, 1.f });
			m_Tangents = &s_Tangents;
		}

		if (!m_UVs)
			return;

		const auto triangleCount = (m_Indices ? m_Indices->size() : m_Positions->size()) / 3;
		std::vector<glm::vec3> tangents(triangleCount * 3);
		std::vector<glm::vec3> bitangents(triangleCount * 3);

		for (int i = 0; i < triangleCount; i++) {
			const auto vIndex = i * 3;
			auto i0 = vIndex;
			auto i1 = vIndex + 1;
			auto i2 = vIndex + 2;

			if (m_Indices) {
				i0 = (*m_Indices)[i0];
				i1 = (*m_Indices)[i1];
				i2 = (*m_Indices)[i2];
			}

			const auto& p0 = (*m_Positions)[i0];
			const auto& p1 = (*m_Positions)[i1];
			const auto& p2 = (*m_Positions)[i2];

			const auto& uv0 = (*m_UVs)[i0];
			const auto& uv1 = (*m_UVs)[i1];
			const auto& uv2 = (*m_UVs)[i2];

			auto e1 = p1 - p0;
			auto e2 = p2 - p0;

			float x1 = uv1.x - uv0.x, x2 = uv2.x - uv0.x,
			      y1 = uv1.y - uv0.y, y2 = uv2.y - uv0.y;

			float det = 1.f / (x1 * y2 - x2 * y1);
			glm::vec3 t = (e1 * y2 - e2 * y1) * det;
			glm::vec3 b = (e2 * x1 - e1 * x2) * det;

			tangents[vIndex + 0] += t;
			tangents[vIndex + 1] += t;
			tangents[vIndex + 2] += t;

			bitangents[vIndex + 0] += b;
			bitangents[vIndex + 1] += b;
			bitangents[vIndex + 2] += b;
		}

		for (auto i = 0; i < m_Positions->size(); i++) {
			// Gram-Schmidt Orthonormalization
			const auto& t = tangents[i];
			const auto& b = bitangents[i];
			const auto& n = (*m_Normals)[i];

			(*m_Tangents)[i] = glm::vec4(glm::normalize(t - glm::dot(t, n) * n), 0.f);
			(*m_Tangents)[i].w = (glm::dot(glm::cross(t, b), n) > 0.f) ? 1.f : -1.f;
		}

	}

	Ref<Mesh> MeshBuilder::BuildMesh()
	{
		BufferLayout layout;
		size_t size = 0;

		APEX_CORE_ASSERT(m_Positions, "Cannot build mesh without position data!"); {
			layout.AddElement(BufferElement{ ShaderDataType::Float3, VertexElementType::Position });
			size += m_Positions->size() * sizeof (*m_Positions)[0];
		}

		if (m_UVs) {
			layout.AddElement(BufferElement{ ShaderDataType::Float2, VertexElementType::UV0 });
			size += (*m_UVs).size() * sizeof (*m_UVs)[0];
		}
		if (m_Normals) {
			layout.AddElement(BufferElement{ ShaderDataType::Float3, VertexElementType::Normal });
			size += (*m_Normals).size() * sizeof (*m_Normals)[0];
		}
		if (m_Tangents) {
			layout.AddElement(BufferElement{ ShaderDataType::Float3, VertexElementType::Tangent });
			size += (*m_Tangents).size() * sizeof (*m_Tangents)[0];
		}
		if (m_Bitangents) {
			layout.AddElement(BufferElement{ ShaderDataType::Float3, VertexElementType::Bitangent });
			size += (*m_Bitangents).size() * sizeof (*m_Bitangents)[0];
		}
		if (m_TexIndexes) {
			layout.AddElement(BufferElement{ ShaderDataType::Float, VertexElementType::TextureIndex });
			size += (*m_TexIndexes).size() * sizeof (*m_TexIndexes)[0];
		}

		size /= sizeof(float);

		std::vector<float> vertices(size);
		float* ptr = vertices.data();
		for (int v = 0; v < m_Positions->size(); v++) {
			
			*reinterpret_cast<glm::vec3*>(ptr) = (*m_Positions)[v];
			ptr += 3;
			
			if (m_UVs) {
				*reinterpret_cast<glm::vec2*>(ptr) = (*m_UVs)[v];
				ptr += 2;
			}
			if (m_Normals) {
				*reinterpret_cast<glm::vec3*>(ptr) = (*m_Normals)[v];
				ptr += 3;
			}
			if (m_Tangents) {
				*reinterpret_cast<glm::vec3*>(ptr) = (*m_Tangents)[v];
				ptr += 3;
			}
			if (m_Bitangents) {
				*reinterpret_cast<glm::vec3*>(ptr) = (*m_Bitangents)[v];
				ptr += 3;
			}
			if (m_TexIndexes) {
				*ptr = (*m_TexIndexes)[v];
				ptr += 1;
			}
		}

		if (m_Indices)
			return CreateRef<Mesh>(vertices.data(), vertices.size(), m_Indices->data(), m_Indices->size(), layout);
		return CreateRef<Mesh>(vertices.data(), vertices.size(), layout);
	}

	void MeshBuilder::__ClearStaticData()
	{
		std::vector<glm::vec3>().swap(s_Normals);
	}
}
