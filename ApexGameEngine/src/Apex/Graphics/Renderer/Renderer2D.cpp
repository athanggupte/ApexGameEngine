#if 0

#include "apex_pch.h"
#include "Renderer2D.h"

#include "Apex/Core/Camera.h"
#include "Apex/Graphics/RenderPrimitives/VertexArray.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"

#include "Renderer.h"

namespace Apex {
	
	struct Renderer2DData
	{
		Ref<VertexArray> quadVertexArray;
		Ref<Shader> textureShader;
		Ref<Texture2D> whiteTexture;
	};
	
	static Renderer2DData* s_RenderData;
	
	void Renderer2D::Init()
	{
		s_RenderData = new Renderer2DData;
		
		// Set the quad vertex array
		s_RenderData->quadVertexArray = VertexArray::Create();
		float squareVertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		auto squareVB = Apex::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
			{ Apex::ShaderDataType::Float3, "a_Position" },
			{ Apex::ShaderDataType::Float2, "a_TexCoord" }
		});
		s_RenderData->quadVertexArray->AddVertexBuffer(squareVB);
		
		uint32_t squareIndices[] = {
			0, 1, 2,
			0, 2, 3
		};
		auto squareIB = Apex::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		s_RenderData->quadVertexArray->AddIndexBuffer(squareIB);
		
		s_RenderData->whiteTexture = Texture2D::Create(1U, 1U, "white");
		uint32_t whiteTextureData = 0xffffffff;
		//s_RenderData->whiteTexture->
	}
	
	//void DrawSimpleQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	
}

#endif
