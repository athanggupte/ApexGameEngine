#include "apex_pch.h"
#include "Renderer2D.h"

#include "Apex.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Apex/Core/Camera.h"
#include "Apex/Graphics/RenderPrimitives/VertexArray.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"

#include "RenderCommands.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Apex {

	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoord;
		float texIndex;
	};

	struct Renderer2DData
	{
		constexpr static const uint32_t MAX_QUADS = 10000;
		constexpr static const uint32_t MAX_VERTICES = MAX_QUADS * 4;
		constexpr static const uint32_t MAX_INDICES = MAX_QUADS * 6;
		constexpr static const uint32_t MAX_TEXTURE_SLOTS = 32;
		constexpr static const uint32_t BASE_TEXTURE_SLOT = 1;
		
		
		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<Shader> textureShader;
		Ref<Texture2D> whiteTexture;
		
		uint32_t quadIndexCount = 0;
		QuadVertex* quadBufferBase = nullptr;
		QuadVertex* quadBufferPtr = nullptr;
		
		std::array<Ref<Texture>, MAX_TEXTURE_SLOTS> textureSlots;
		uint32_t textureSlotIndex = BASE_TEXTURE_SLOT;
		
		Renderer2D::Stats stats;
		
		constexpr static glm::vec4 quadVertexPositions[4] = {
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f }
		};
		
		constexpr static glm::vec2 quadVertexTexCoords[4] = {
			{ 0.f, 0.f },
			{ 1.f, 0.f },
			{ 1.f, 1.f },
			{ 0.f, 1.f }
		};
	};

	static Renderer2DData s_RenderData;

	void Renderer2D::Init()
	{
		// Create vertex array (VAO)
		s_RenderData.quadVertexArray = VertexArray::Create();
		
		// Create vertex buffer (VBO), and set to VAO
		s_RenderData.quadBufferBase = new QuadVertex[Renderer2DData::MAX_VERTICES];
		
		s_RenderData.quadVertexBuffer = VertexBuffer::Create(Renderer2DData::MAX_VERTICES * sizeof(QuadVertex));
		s_RenderData.quadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, VertexElementType::Position },
			{ ShaderDataType::Float4, VertexElementType::Color },
			{ ShaderDataType::Float2, VertexElementType::UV0 },
			{ ShaderDataType::Float,  VertexElementType::TextureIndex }
		});
		s_RenderData.quadVertexArray->AddVertexBuffer(s_RenderData.quadVertexBuffer);
		
		// Create index buffer (IBO), and set to VAO
		uint32_t* quadIndices = new uint32_t[Renderer2DData::MAX_INDICES];
		for (uint32_t i = 0, offset = 0; i < Renderer2DData::MAX_INDICES; i += 6, offset += 4) {
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;
			
			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;
		}
		auto quadIB = IndexBuffer::Create(quadIndices, Renderer2DData::MAX_INDICES);
		s_RenderData.quadVertexArray->AddIndexBuffer(quadIB);
		delete[] quadIndices;
		
		// Placeholder texture
		auto whiteTextureSpec = TextureSpec{ TextureAccessFormat::RGBA, TextureInternalFormat::RGBA8, TextureDataType::UBYTE };
		s_RenderData.whiteTexture = Texture2D::Create(1U, 1U, whiteTextureSpec, "white");
		uint32_t whiteTextureData = 0xffffffff;
		s_RenderData.whiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));
		
		// Create shader
		auto textureVertexSrc = R"(
			#version 450

			layout(location = ATTRIB_LOC_Position) in vec3 a_Position;
			layout(location = ATTRIB_LOC_Color) in vec4 a_Color;
			layout(location = ATTRIB_LOC_UV0) in vec2 a_TexCoord;
			layout(location = ATTRIB_LOC_TexIndex) in float a_TexIndex;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;
			out vec2 v_TexCoord;
			out flat float v_TexIndex;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				v_TexCoord = vec2(a_TexCoord.x, a_TexCoord.y * -1.0);
				v_TexIndex = a_TexIndex;
				
				gl_Position = u_ViewProjection /** u_Transform*/ * vec4(a_Position, 1.0);
			}
		)";
		
		auto textureFragmentSrc = R"(
			#version 450

			layout(location = 0) out vec4 o_Color;

			uniform sampler2D u_Textures[)" + std::to_string(Renderer2DData::MAX_TEXTURE_SLOTS) + R"(];
			//uniform float u_TilingFactor;

			in vec3 v_Position;
			in vec4 v_Color;
			in vec2 v_TexCoord;
			in flat float v_TexIndex;

			void main()
			{
				o_Color = texture(u_Textures[int(v_TexIndex)], v_TexCoord /** u_TilingFactor*/).rgba * v_Color;
				if (o_Color.a < 0.05)
					discard;
			}
		)";
		
		s_RenderData.textureShader = Shader::Create("Textured Quad Shader", textureVertexSrc, textureFragmentSrc);
		s_RenderData.textureShader->Bind();
		
		int textureUniforms[Renderer2DData::MAX_TEXTURE_SLOTS];
		for (auto i=0; i<Renderer2DData::MAX_TEXTURE_SLOTS; i++) {
			textureUniforms[i] = i;
		}
		
		s_RenderData.textureShader->SetUniInt1v("u_Textures", textureUniforms, Renderer2DData::MAX_TEXTURE_SLOTS);
		
		s_RenderData.textureSlots[0] = s_RenderData.whiteTexture;
	}

	void Renderer2D::Shutdown()
	{
		//delete s_RenderData;
		delete[] s_RenderData.quadBufferBase;
	}
	
	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		auto viewProj = camera.GetProjection() * glm::inverse(transform);
		
		s_RenderData.textureShader->Bind();
		s_RenderData.textureShader->SetUniMat4("u_ViewProjection", viewProj);
		
		ResetBatch();
	}
	
	void Renderer2D::ResetBatch()
	{
		s_RenderData.quadIndexCount = 0;
		s_RenderData.quadBufferPtr = s_RenderData.quadBufferBase;
		s_RenderData.textureSlotIndex = Renderer2DData::BASE_TEXTURE_SLOT;
	}
	
	void Renderer2D::EndScene()
	{
		size_t dataSize = (uint8_t*)s_RenderData.quadBufferPtr - (uint8_t*)s_RenderData.quadBufferBase;
		s_RenderData.quadVertexBuffer->SetData(s_RenderData.quadBufferBase, dataSize);

		if (dataSize > 0)
			FlushBatch();
	}

	void Renderer2D::FlushBatch()
	{
		/*static bool firstRun = true;
		if (firstRun) {
			APEX_CORE_DEBUG("textureSlots:");
			for (auto i=0; i < Renderer2DData::MAX_TEXTURE_SLOTS; i++)
				APEX_CORE_DEBUG("slot: {0} | id: {1}", i, s_RenderData.textureSlots[i] ? s_RenderData.textureSlots[i]->GetID() : 0);
			auto count = 0;
			for (auto it=s_RenderData.quadBufferBase; it < s_RenderData.quadBufferPtr; it+=4)
				APEX_CORE_DEBUG("quad: {0} | id: {1}", count++, it->texIndex);
			
			firstRun = false;
		}*/
		
		//APEX_CORE_DEBUG("Number of textures bound: {}", s_RenderData.textureSlotIndex);
		s_RenderData.textureShader->Bind();
		for (uint32_t i = 0; i < s_RenderData.textureSlotIndex; i++) {
			s_RenderData.textureSlots[i]->Bind(i);
		}
		RenderCommands::DrawIndexed(s_RenderData.quadVertexArray, s_RenderData.quadIndexCount);
		
		// Statistics
		s_RenderData.stats.drawCalls++;
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
		if (s_RenderData.quadIndexCount >= Renderer2DData::MAX_INDICES)
		{
			EndScene();
			ResetBatch();
		}
		
		constexpr float whiteTextureIndex = 0.f;
		
		for (uint32_t i=0; i<4; i++) {
			s_RenderData.quadBufferPtr->position = transform * s_RenderData.quadVertexPositions[i];
			s_RenderData.quadBufferPtr->color = color;
			s_RenderData.quadBufferPtr->texCoord = s_RenderData.quadVertexTexCoords[i];
			s_RenderData.quadBufferPtr->texIndex = whiteTextureIndex;
			s_RenderData.quadBufferPtr++;
		}
		
		s_RenderData.quadIndexCount += 6;
		
		// Statistics
		s_RenderData.stats.quadCount++;
	}

	// Transform matrix, textured
	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		if (s_RenderData.quadIndexCount >= Renderer2DData::MAX_INDICES)
		{
			EndScene();
			ResetBatch();
		}
		
		constexpr glm::vec4 color = { 1.f, 1.f, 1.f, 1.f };
		
		float textureIndex = 0.f;
		
		for (uint32_t i = s_RenderData.BASE_TEXTURE_SLOT; i < s_RenderData.textureSlotIndex; i++) {
			if (s_RenderData.textureSlots[i]->GetID() == texture->GetID()) {
				textureIndex = static_cast<float>(i);
				break;
			}
		}
		
		if (textureIndex == 0.f)
		{
			textureIndex = static_cast<float>(s_RenderData.textureSlotIndex);
			s_RenderData.textureSlots[s_RenderData.textureSlotIndex] = texture;
			s_RenderData.textureSlotIndex++;
		}
		
		for (uint32_t i=0; i<4; i++) {
			s_RenderData.quadBufferPtr->position = transform * s_RenderData.quadVertexPositions[i];
			s_RenderData.quadBufferPtr->color = color;
			s_RenderData.quadBufferPtr->texCoord = s_RenderData.quadVertexTexCoords[i] * tilingFactor;
			s_RenderData.quadBufferPtr->texIndex = textureIndex;
			s_RenderData.quadBufferPtr++;
		}
		
		s_RenderData.quadIndexCount += 6;
		
		// Statistics
		s_RenderData.stats.quadCount++;
	}
	
	void Renderer2D::ResetStats()
	{
		memset(&s_RenderData.stats, 0, sizeof(Stats));
	}
	
	Renderer2D::Stats Renderer2D::GetStats()
	{
		return s_RenderData.stats;
	}
	
}
