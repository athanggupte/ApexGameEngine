#include "apex_pch.h"
#include "Mesh.h"

#include "Apex/Graphics/RenderPrimitives/Buffer.h"

namespace Apex {

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

	//void Mesh::AddTexture(std::pair<std::string, Ref<Texture>>& nameTexturePair)
	//{
	//	m_Textures.insert(nameTexturePair);
	//}

	//void Mesh::AddTexture(std::string name, Ref<Texture> texture)
	//{
	//	m_Textures[name] = texture;
	//}


}
