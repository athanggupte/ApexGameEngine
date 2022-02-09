#include "apex_pch.h"
#include "Font.h"

#include "imgui.h"
#include "RenderPrimitives/Texture.h"

namespace Apex {

	// Font Glyph //
	glm::vec2 FontGlyph::GetPosition(Corner corner) const
	{
		glm::vec2 position { 0.f };
		switch (corner)
		{
		case BOT_LEFT:
			position.x = m_Glyph->X0;
			position.y = m_Glyph->Y0;
			break;
		case BOT_RIGHT:
			position.x = m_Glyph->X1;
			position.y = m_Glyph->Y0;
			break;
		case TOP_RIGHT:
			position.x = m_Glyph->X1;
			position.y = m_Glyph->Y1;
			break;
		case TOP_LEFT:
			position.x = m_Glyph->X0;
			position.y = m_Glyph->Y1;
			break;
		}
		position -= 0.5f;
		return position;
	}

	glm::vec2 FontGlyph::GetUV(Corner corner) const
	{
		glm::vec2 uv { 0.f };
		switch (corner)
		{
		case BOT_LEFT:
			uv.x = m_Glyph->U0;
			uv.y = m_Glyph->V0;
			break;
		case BOT_RIGHT:
			uv.x = m_Glyph->U1;
			uv.y = m_Glyph->V0;
			break;
		case TOP_RIGHT:
			uv.x = m_Glyph->U1;
			uv.y = m_Glyph->V1;
			break;
		case TOP_LEFT:
			uv.x = m_Glyph->U0;
			uv.y = m_Glyph->V1;
			break;
		}
		return uv;
	}

	float FontGlyph::GetAdvanceX() const
	{
		return m_Glyph->AdvanceX;
	}

	FontGlyph::FontGlyph(const ImFontGlyph* glyph)
		: m_Glyph(glyph)
	{
	}

	// Font //
	Font::Font(ImFont* font, const FontAtlas* font_atlas)
		: m_Font(font), m_ParentFontAtlas(font_atlas)
	{
	}

	FontGlyph Font::FindGlyph(uint16_t c) const
	{
		return { m_Font->FindGlyph(c) };
	}

	const FontAtlas* Font::GetFontAtlas() const
	{
		return m_ParentFontAtlas;
	}

	float Font::GetFontSize() const
	{
		return m_Font->FontSize;
	}

	// Font Atlas //
	FontAtlas::FontAtlas()
		: m_FontAtlas(new ImFontAtlas())
	{
		m_FontAtlas->Flags |= ImFontAtlasFlags_NoMouseCursors;
	}

	FontAtlas::~FontAtlas()
	{
		delete m_FontAtlas;
	}

	Font FontAtlas::AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg,
	                                   const uint16_t* glyph_ranges) const
	{
		return { m_FontAtlas->AddFontFromFileTTF(filename, size_pixels, font_cfg, glyph_ranges), this };
	}

	Font FontAtlas::AddFontFromMemoryCompressedTTF(const void* compressed_font_data, int compressed_font_size,
	                                               float size_pixels, const ImFontConfig* font_cfg,
	                                               const uint16_t* glyph_ranges) const
	{
		return { m_FontAtlas->AddFontFromMemoryCompressedTTF(compressed_font_data, compressed_font_size, size_pixels, font_cfg, glyph_ranges), this };
	}

	Font FontAtlas::AddFontFromMemoryCompressedBase85TTF(const char* compressed_font_data_base85,
	                                                     float size_pixels,
	                                                     const ImFontConfig* font_cfg,
	                                                     const uint16_t* glyph_ranges) const
	{
		return { m_FontAtlas->AddFontFromMemoryCompressedBase85TTF(compressed_font_data_base85, size_pixels, font_cfg, glyph_ranges), this };
	}

	Ref<Texture2D> FontAtlas::GetFontAtlasTexture() const
	{
		return m_Texture;
	}

	void FontAtlas::BuildAlpha8()
	{
		uint8_t* pixelData;
		int width, height;

		m_FontAtlas->GetTexDataAsAlpha8(&pixelData, &width, &height);
		m_Texture = Texture2D::Create(width, height, { TextureAccessFormat::RED, TextureInternalFormat::R8, TextureDataType::UBYTE, TextureFiltering::BILINEAR }, "Font Atlas");
		m_Texture->SetData(pixelData, width * height);
	}

	void FontAtlas::BuildRGBA32()
	{
		uint8_t* pixelData;
		int width, height;

		m_FontAtlas->GetTexDataAsRGBA32(&pixelData, &width, &height);
		m_Texture = Texture2D::Create(width, height, { TextureAccessFormat::RGBA, TextureInternalFormat::RGBA8, TextureDataType::UBYTE, TextureFiltering::BILINEAR }, "Font Atlas");
		m_Texture->SetData(pixelData, width * height * 4);
	}

	Font FontAtlas::GetFontAtIndex(int32_t index)
	{
		return { m_FontAtlas->Fonts[index], this };
	}
}
