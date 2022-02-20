#pragma once

#include "glm/glm.hpp"
#include "Apex/Graphics/RenderPrimitives/VertexArray.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"

namespace fs = std::filesystem;

namespace Apex {

	class APEX_API Mesh
	{
	public:
		//Mesh(const fs::path& filepath);
		Mesh(float* vertices, size_t vCount, uint32_t* indices, size_t iCount, const BufferLayout& layout);
		Mesh(float* vertices, size_t vCount, const BufferLayout& layout);
		virtual ~Mesh() = default;

		virtual inline void Bind() const { m_VAO->Bind(); }
		virtual inline const Ref<VertexArray>& GetVAO() const { return m_VAO; }

		virtual inline bool& Show() { return m_Show; }
		virtual inline const bool& Show() const { return m_Show; }

		// virtual inline void SetTextures(std::unordered_map<std::string, Ref<Texture>> textures) { m_Textures = textures; }
		// virtual inline const std::unordered_map<std::string, Ref<Texture>>& GetTextures() const { return m_Textures; }
		
		//virtual void SetTexture(std::pair<std::string, Ref<Texture>>& nameTexturePair);
		//virtual void SetTexture(std::string name, Ref<Texture> texture);

		virtual inline bool HasNormals() const { return m_HasNormals; }
		virtual inline bool HasTangents() const { return m_HasTangents; }

	private:
		Ref<VertexArray> m_VAO;
		// std::unordered_map<std::string, Ref<Texture>> m_Textures;
		bool m_Show = true;
		
		bool m_HasNormals = true;
		bool m_HasTangents = false;
		
		friend class Model;
	};


	class APEX_API MeshBuilder
	{
	public:
		MeshBuilder();

		// Setters
		void SetIndices(std::vector<uint32_t>& indices) { m_Indices = &indices; }				// non-owning pointer; must be valid until BuildMesh() is called
		void SetPositions(std::vector<glm::vec3>& positions) { m_Positions = &positions; }		// non-owning pointer; must be valid until BuildMesh() is called
		void SetUVs(std::vector<glm::vec2>& uvs) { m_UVs = &uvs; }								// non-owning pointer; must be valid until BuildMesh() is called
		void SetNormals(std::vector<glm::vec3>& normals) { m_Normals = &normals; }				// non-owning pointer; must be valid until BuildMesh() is called
		void SetTangents(std::vector<glm::vec4>& tangents) { m_Tangents = &tangents; }			// non-owning pointer; must be valid until BuildMesh() is called
		void SetBitangents(std::vector<glm::vec3>& bitangents) { m_Bitangents = &bitangents; }	// non-owning pointer; must be valid until BuildMesh() is called
		void SetTexIndexes(std::vector<float>& tex_indexes) { m_TexIndexes = &tex_indexes; }	// non-owning pointer; must be valid until BuildMesh() is called

		// Modifiers
		void RegenerateSoftNormals(); // Call after setting all attributes
		void RegenerateFlatNormals(); // Call after setting all attributes
		void RegenerateTangents(); // Call after generating or setting normals and other attributes

		// Build
		Ref<Mesh> BuildMesh();

		// Control
		static void __ClearStaticData();

	private:
		std::vector<uint32_t>* m_Indices;
		std::vector<glm::vec3>* m_Positions;
		std::vector<glm::vec2>* m_UVs;
		std::vector<glm::vec3>* m_Normals;
		std::vector<glm::vec4>* m_Tangents;
		std::vector<glm::vec3>* m_Bitangents;
		std::vector<float>* m_TexIndexes;
	};

}
