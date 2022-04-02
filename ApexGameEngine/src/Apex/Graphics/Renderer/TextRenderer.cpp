#include "apex_pch.h"
#include "TextRenderer.h"

#include "Apex.h"
#include "Apex.h"
#include "Apex.h"
#include "Apex.h"
#include "Apex.h"
#include "Apex.h"
#include "Apex.h"
#include "Apex.h"
#include "Apex.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Apex/Core/Camera.h"
#include "Apex/Graphics/RenderPrimitives/VertexArray.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"

#include "RenderCommands.h"
#include "Apex/Graphics/Font.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Apex {

	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoord;
		float texIndex;
		float fontWidth;
		float fontThickness;
		float __extra__{0.f};
	};

	struct TextRendererData
	{
		constexpr static const uint32_t MAX_QUADS = 10000;
		constexpr static const uint32_t MAX_VERTICES = MAX_QUADS * 4;
		constexpr static const uint32_t MAX_INDICES = MAX_QUADS * 6;
		constexpr static const uint32_t MAX_TEXTURE_SLOTS = 32;
		constexpr static const uint32_t BASE_TEXTURE_SLOT = 1;
		
		
		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<Shader> textSDFShader;
		Ref<Texture2D> whiteTexture;
		
		uint32_t quadIndexCount = 0;
		QuadVertex* quadBufferBase = nullptr;
		QuadVertex* quadBufferPtr = nullptr;
		
		std::array<Ref<Texture>, MAX_TEXTURE_SLOTS> textureSlots;
		uint32_t textureSlotIndex = BASE_TEXTURE_SLOT;
		
		TextRenderer::Stats stats;
		
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

	static TextRendererData s_RenderData;

	void TextRenderer::Init()
	{
		// Create vertex array (VAO)
		s_RenderData.quadVertexArray = VertexArray::Create();
		
		// Create vertex buffer (VBO), and set to VAO
		s_RenderData.quadBufferBase = new QuadVertex[TextRendererData::MAX_VERTICES];
		
		s_RenderData.quadVertexBuffer = VertexBuffer::Create(TextRendererData::MAX_VERTICES * sizeof(QuadVertex));
		s_RenderData.quadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, VertexElementType::Position },
			{ ShaderDataType::Float4, VertexElementType::Color },
			{ ShaderDataType::Float2, VertexElementType::UV0 },
			{ ShaderDataType::Float4,  VertexElementType::TextureIndex }, // TexIndex (1) | Width (1) | Thickness (1)
		});
		s_RenderData.quadVertexArray->AddVertexBuffer(s_RenderData.quadVertexBuffer);
		
		{ // Create index buffer (IBO), and set to VAO
			std::array<uint32_t, TextRendererData::MAX_INDICES> quadIndices{};
			for (uint32_t i = 0, offset = 0; i < TextRendererData::MAX_INDICES; i += 6, offset += 4) {
				quadIndices[i + 0] = offset + 0;
				quadIndices[i + 1] = offset + 1;
				quadIndices[i + 2] = offset + 2;
				
				quadIndices[i + 3] = offset + 2;
				quadIndices[i + 4] = offset + 3;
				quadIndices[i + 5] = offset + 0;
			}
			auto quadIB = IndexBuffer::Create(quadIndices.data(), TextRendererData::MAX_INDICES);
			s_RenderData.quadVertexArray->AddIndexBuffer(quadIB);
			// delete[] quadIndices;
		}
		
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
			layout(location = ATTRIB_LOC_TexIndex) in vec4 a_TexIndex;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;
			out vec2 v_TexCoord;
			out flat vec4 v_TexIndex;

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

			uniform sampler2D u_Textures[)" + std::to_string(TextRendererData::MAX_TEXTURE_SLOTS) + R"(];
			//uniform float u_TilingFactor;

			in vec3 v_Position;
			in vec4 v_Color;
			in vec2 v_TexCoord;
			in flat vec4 v_TexIndex;

			//const float width = 0.52;
			const float edge = 0.02;

			void main()
			{
				float width = v_TexIndex.y;
				float distance = 1.0 - texture(u_Textures[int(v_TexIndex.x)], v_TexCoord).a;
				float alpha = 1.0 - smoothstep(width, width + edge, distance);
				o_Color = vec4(v_Color.rgb, alpha);

				if (o_Color.a < 0.05)
					discard;
			}
		)";
		
		s_RenderData.textSDFShader = Shader::Create("Textured Quad Shader", textureVertexSrc, textureFragmentSrc);
		s_RenderData.textSDFShader->Bind();
		
		int textureUniforms[TextRendererData::MAX_TEXTURE_SLOTS];
		for (auto i=0; i<TextRendererData::MAX_TEXTURE_SLOTS; i++) {
			textureUniforms[i] = i;
		}
		
		s_RenderData.textSDFShader->SetUniInt1v("u_Textures", textureUniforms, TextRendererData::MAX_TEXTURE_SLOTS);
		
		s_RenderData.textureSlots[0] = s_RenderData.whiteTexture;
	}

	void TextRenderer::Shutdown()
	{
		//delete s_RenderData;
		delete[] s_RenderData.quadBufferBase;
	}
	
	void TextRenderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		auto viewProj = camera.GetProjection() * glm::inverse(transform);
		
		s_RenderData.textSDFShader->Bind();
		s_RenderData.textSDFShader->SetUniMat4("u_ViewProjection", viewProj);
		
		ResetBatch();
	}
	
	void TextRenderer::ResetBatch()
	{
		s_RenderData.quadIndexCount = 0;
		s_RenderData.quadBufferPtr = s_RenderData.quadBufferBase;
		s_RenderData.textureSlotIndex = TextRendererData::BASE_TEXTURE_SLOT;
	}
	
	void TextRenderer::EndScene()
	{
		size_t dataSize = (uint8_t*)s_RenderData.quadBufferPtr - (uint8_t*)s_RenderData.quadBufferBase;
		s_RenderData.quadVertexBuffer->SetData(s_RenderData.quadBufferBase, dataSize);

		if (dataSize > 0)
			FlushBatch();
	}

	void TextRenderer::FlushBatch()
	{
		/*static bool firstRun = true;
		if (firstRun) {
			APEX_CORE_DEBUG("textureSlots:");
			for (auto i=0; i < TextRendererData::MAX_TEXTURE_SLOTS; i++)
				APEX_CORE_DEBUG("slot: {0} | id: {1}", i, s_RenderData.textureSlots[i] ? s_RenderData.textureSlots[i]->GetID() : 0);
			auto count = 0;
			for (auto it=s_RenderData.quadBufferBase; it < s_RenderData.quadBufferPtr; it+=4)
				APEX_CORE_DEBUG("quad: {0} | id: {1}", count++, it->texIndex);
			
			firstRun = false;
		}*/
		
		//APEX_CORE_DEBUG("Number of textures bound: {}", s_RenderData.textureSlotIndex);
		s_RenderData.textSDFShader->Bind();
		for (uint32_t i = 0; i < s_RenderData.textureSlotIndex; i++) {
			s_RenderData.textureSlots[i]->Bind(i);
		}
		RenderCommands::DrawIndexed(s_RenderData.quadVertexArray, s_RenderData.quadIndexCount);
		
		// Statistics
		s_RenderData.stats.drawCalls++;
	}

	void TextRenderer::DrawGlyphs(const ::std::string& text, const glm::mat4& transform, const ::Apex::Font& font, const glm::vec4& color, float width, float thickness)
	{
		const auto numIndexCountForText = text.length() * 6;
		if (s_RenderData.quadIndexCount + numIndexCountForText >= TextRendererData::MAX_INDICES)
		{
			EndScene();
			ResetBatch();
		}

		auto texture = font.GetFontAtlas()->GetFontAtlasTexture();
				
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

		auto x = 0.f, y = 0.f;

		// Taken from imgui_draw.cpp > ImFont::RenderText
		auto s = text.data();
		auto s_end = text.data() + text.size();
		while (s < s_end) {
			auto c = static_cast<uint32_t>(*s);
			if (c < 0x80) {
				s += 1;
			} else {
				s += ImTextCharFromUtf8(&c, s, s_end);
				if (c == 0) // Malformed UTF-8?
					break;
			}

			auto glyph = font.FindGlyph(c);
			for (uint32_t i=0; i<4; i++) {
				auto position = glyph.GetPosition(static_cast<FontGlyph::Corner>(i)) / font.GetFontSize();
				auto uv = glyph.GetUV(static_cast<FontGlyph::Corner>(i));

				s_RenderData.quadBufferPtr->position = transform * glm::vec4{ x + position.x, y - position.y, 0.f, 1.f };
				s_RenderData.quadBufferPtr->color = color;
				s_RenderData.quadBufferPtr->texCoord = { uv.x, -uv.y };
				s_RenderData.quadBufferPtr->texIndex = textureIndex;
				s_RenderData.quadBufferPtr->fontWidth = width;
				s_RenderData.quadBufferPtr->fontThickness = thickness;
				s_RenderData.quadBufferPtr++;
			}
			s_RenderData.quadIndexCount += 6;
			x += glyph.GetAdvanceX() / font.GetFontSize();
		
			// Statistics
			s_RenderData.stats.quadCount++;
		}
	}
	
	void TextRenderer::ResetStats()
	{
		memset(&s_RenderData.stats, 0, sizeof(Stats));
	}
	
	TextRenderer::Stats TextRenderer::GetStats()
	{
		return s_RenderData.stats;
	}
	
}
