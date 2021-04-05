#include "apex_pch.h"
#include "Renderer2D.h"

#include "Apex/Core/Camera.h"
#include "Apex/Graphics/RenderPrimitives/VertexArray.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"

#include "RenderCommands.h"

#include <glm/gtc/matrix_transform.hpp>

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
		
		// Create vertex array (VAO)
		s_RenderData->quadVertexArray = VertexArray::Create();
		
		// Create Quad vertices
		float squareVertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		
		// Create vertex buffer (VBO), and set to VAO
		auto squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		});
		s_RenderData->quadVertexArray->AddVertexBuffer(squareVB);
		
		// Create Quad indices
		uint32_t squareIndices[] = {
			0, 1, 2,
			0, 2, 3
		};
		
		// Create index buffer (IBO), and set to VAO
		auto squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		s_RenderData->quadVertexArray->AddIndexBuffer(squareIB);
		
		// Placeholder texture
		s_RenderData->whiteTexture = Texture2D::Create(1U, 1U, "white");
		uint32_t whiteTextureData = 0xffffffff;
		
		// Create shader
		auto textureVertexSrc = R"(
			#version 450

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec2 v_TexCoord;

			void main()
			{
				v_Position = a_Position;
				v_TexCoord = vec2(a_TexCoord.x, a_TexCoord.y * -1.0);
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";
		
		auto textureFragmentSrc = R"(
			#version 450

			layout(location = 0) out vec4 o_Color;

			uniform vec4 u_Color;
			uniform sampler2D u_Texture;
			uniform float u_TilingFactor;
			uniform bool u_UseTexture;

			in vec3 v_Position;
			in vec2 v_TexCoord;

			void main()
			{
				if (u_UseTexture) {
					o_Color = texture(u_Texture, v_TexCoord * u_TilingFactor).rgba * u_Color;
				} else {
					o_Color = u_Color;
				}
			}
		)";
		
		s_RenderData->textureShader = Shader::Create("Quad-Texture", textureVertexSrc, textureFragmentSrc);
		s_RenderData->textureShader->SetUniInt1("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		delete s_RenderData;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_RenderData->textureShader->Bind();
		s_RenderData->textureShader->SetUniMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	}

	void Renderer2D::EndScene()
	{
		// Do things at end of scene
	}

	// 2D coords, flat color
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(glm::vec3{ position.x, position.y, 0.f }, size, color);
	}

	// 3D coords, flat color
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		auto transform = glm::translate(glm::mat4(1.f), position)
						* glm:: scale(glm::mat4(1.f), { size.x, size.y, 1.f });
		DrawQuad(transform, color);
	}

	// 2D coords, textured
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad(glm::vec3{ position.x, position.y, 0.f }, size, texture, tilingFactor, tintColor);
	}

	// 3D coords, textured
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		auto transform = glm::translate(glm::mat4(1.f), position)
						* glm:: scale(glm::mat4(1.f), { size.x, size.y, 1.f });
		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	// 2D coords, 2D rotation, flat color
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawQuad(glm::vec3{ position.x, position.y, 0.f }, size, rotation, color);
	}

	// 3D coords, 2D rotation, flat color
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		auto transform = glm::translate(glm::mat4(1.f), position)
						* glm::rotate(glm::mat4(1.f), rotation, { 0.f, 0.f, 1.f })
						* glm:: scale(glm::mat4(1.f), { size.x, size.y, 1.f });
		DrawQuad(transform, color);
	}

	// 2D coords, 2D rotation, textured
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad(glm::vec3{ position.x, position.y, 0.f }, size, rotation, texture, tilingFactor, tintColor);
	}

	// 3D coords, 2D rotation, textured
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		auto transform = glm::translate(glm::mat4(1.f), position)
						* glm::rotate(glm::mat4(1.f), rotation, { 0.f, 0.f, 1.f })
						* glm:: scale(glm::mat4(1.f), { size.x, size.y, 1.f });
		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	// Transform matrix, flat color
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		// Bind Shader
		s_RenderData->textureShader->Bind();
		s_RenderData->textureShader->SetUniInt1("u_UseTexture", false);
		
		// Set Transform matrix
		s_RenderData->textureShader->SetUniMat4("u_Transform", transform);
		
		// Set Color
		s_RenderData->textureShader->SetUniFloat4("u_Color", color);
		
		// Bind VAO and submit draw call
		s_RenderData->quadVertexArray->Bind();
		RenderCommands::DrawIndexed(s_RenderData->quadVertexArray);
	}

	// Transform matrix, textured
	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		// Bind Shader
		s_RenderData->textureShader->Bind();
		s_RenderData->textureShader->SetUniInt1("u_UseTexture", true);
		
		// Set Uniforms
		s_RenderData->textureShader->SetUniMat4("u_Transform", transform);
		s_RenderData->textureShader->SetUniFloat4("u_Color", tintColor);
		s_RenderData->textureShader->SetUniFloat1("u_TilingFactor", tilingFactor);
		
		// Bind Texture
		texture->Bind();
		
		// Bind VAO and submit draw call
		s_RenderData->quadVertexArray->Bind();
		RenderCommands::DrawIndexed(s_RenderData->quadVertexArray);
	}
	
}
