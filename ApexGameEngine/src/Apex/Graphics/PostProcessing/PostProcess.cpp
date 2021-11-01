#include "apex_pch.h"
#include "PostProcess.h"

#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/VertexArray.h"
#include "Apex/Graphics/Renderer/RenderCommands.h"

namespace Apex {
	
	struct PostProcessData
	{
		Ref<VertexArray> screenVertexArray;
		//Ref<Shader> postProcessShader;
		//Ref<Texture2D> whiteTexture;
	};
	
	static PostProcessData* s_Data;
	
	void PostProcess::Init()
	{
		s_Data = new PostProcessData;
		
		// Create vertex array (VAO)
		s_Data->screenVertexArray = VertexArray::Create();
		
		// Create screen vertices
		float screenVertices[] = {
			-1.f,  1.f,  0.f, 1.f,
			-1.f, -1.f,  0.f, 0.f,
			 1.f, -1.f,  1.f, 0.f,
		
			-1.f,  1.f,  0.f, 1.f,
			 1.f, -1.f,  1.f, 0.f,
			 1.f,  1.f,  1.f, 1.f
		};
		
		// Create vertex buffer (VBO), and set to VAO
		auto screenVB = VertexBuffer::Create(screenVertices, sizeof(screenVertices));
		screenVB->SetLayout({
			{ ShaderDataType::Float2, VertexElementType::Position },
			{ ShaderDataType::Float2, VertexElementType::TextureCoords }
		});
		s_Data->screenVertexArray->AddVertexBuffer(screenVB);
		
	}
	
	void PostProcess::Shutdown()
	{
		delete s_Data;
	}
	
	void PostProcess::Submit(const Ref<Shader>& shader)
	{
		shader->Bind();
		s_Data->screenVertexArray->Bind();
		RenderCommands::Draw(s_Data->screenVertexArray);
	}
}
