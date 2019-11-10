#include "apex_pch.h"
#include "Mesh.h"

#include "Apex/Renderer/Buffer.h"

namespace Apex {

	Mesh::Mesh(float* vertices, size_t vCount, uint32_t* indices, size_t iCount)
	{
		m_VAO = VertexArray::Create();
		Ref<VertexBuffer> vbo = VertexBuffer::Create(vertices, vCount * sizeof(float));
		Ref<IndexBuffer> ibo = IndexBuffer::Create(indices, iCount);

		vbo->SetLayout({
			{ Apex::ShaderDataType::Float3, "a_Position" },
			{ Apex::ShaderDataType::Float2, "a_TexCoord" }
			//{ Apex::ShaderDataType::Float3, "a_Normal" }
		});

		m_VAO->AddVertexBuffer(vbo);
		m_VAO->AddIndexBuffer(ibo);
		
		m_VAO->Unbind();
	}

}