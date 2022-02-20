#pragma once
#include "glm/vec2.hpp"

struct ImFontGlyph;
struct ImFontConfig;
struct ImFont;
struct ImFontAtlas;

namespace Apex {
	class FontAtlas;
	class Texture2D;

	class FontGlyph
	{
	public:
		enum Corner
		{
			TOP_LEFT,	// 0,0
			TOP_RIGHT,	// 1,0
			BOT_RIGHT,	// 1,1
			BOT_LEFT,	// 0,1
		};

		glm::vec2 GetPosition(Corner corner) const;
		glm::vec2 GetUV(Corner corner) const;
		float GetAdvanceX() const;

	private:
		FontGlyph(const ImFontGlyph* glyph);

	private:
		const ImFontGlyph* m_Glyph;

		friend class Font;
	};

	class Font
	{
	public:
		[[nodiscard]] FontGlyph FindGlyph(uint16_t c) const;
		[[nodiscard]] const FontAtlas* GetFontAtlas() const;
		float GetFontSize() const;

	private:
		Font(ImFont* font, const FontAtlas* font_atlas);

	private:
		ImFont* m_Font = nullptr;
		const FontAtlas* m_ParentFontAtlas;

		friend class FontAtlas;
	};

	class FontAtlas
	{
	public:
		FontAtlas();
		~FontAtlas();

		Font AddFontFromFileTTF(const char* filename, float size_pixels, ImFontConfig* font_cfg = &s_FontConfig, const uint16_t* glyph_ranges = NULL) const;
		Font AddFontFromMemoryCompressedTTF(const void* compressed_font_data, int compressed_font_size, float size_pixels, ImFontConfig* font_cfg = &s_FontConfig, const uint16_t* glyph_ranges = NULL) const; // 'compressed_font_data' still owned by caller. Compress with binary_to_compressed_c.cpp.
		Font AddFontFromMemoryCompressedBase85TTF(const char* compressed_font_data_base85, float size_pixels, ImFontConfig* font_cfg = &s_FontConfig, const uint16_t* glyph_ranges = NULL) const;              // 'compressed_font_data_base85' still owned by caller. Compress with binary_to_compressed

		[[nodiscard]] Ref<Texture2D> GetFontAtlasTexture() const;

		void BuildAlpha8();
		void BuildRGBA32();

		Font GetFontAtIndex(int32_t index);

	private:
		ImFontAtlas* m_FontAtlas;
		Ref<Texture2D> m_Texture;

		static ImFontConfig s_FontConfig;
	};

}
