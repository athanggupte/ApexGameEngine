#include "apexed_pch.h"
#include "EditorFonts.h"

#include "IconsMaterialDesign.h"
#include "imgui.h"

extern const char font_cousine_compressed_data_base85[];

namespace Apex {

	static constexpr float text_font_size = 16.f;
	static constexpr float icon_font_size = 20.f;
	static constexpr float window_icon_font_size = 10.f;
	static constexpr float font_size_diff = icon_font_size - text_font_size;

	void SetupEditorFonts()
	{
		auto& io = ImGui::GetIO();

#ifdef APEX_PLATFORM_WINDOWS
		ImFontConfig font_cfg;
		font_cfg.OversampleH = 7;
		auto ui_font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", text_font_size, &font_cfg);
#endif

		constexpr static ImWchar icon_ranges[] = { ICON_MIN_MD, ICON_MAX_MD, 0 };
		ImFontConfig icon_config;
		icon_config.MergeMode = true;
		icon_config.PixelSnapH = true;
		icon_config.OversampleH = 6;
		icon_config.GlyphMinAdvanceX = icon_font_size;
		icon_config.GlyphOffset = { -2.f, 5.f } /*{ font_size_diff * -0.5f, font_size_diff + 1.f }*/;
		auto icon_font = io.Fonts->AddFontFromFileTTF( "assets/fonts/" FONT_ICON_FILE_NAME_MD, icon_font_size, &icon_config, icon_ranges );

		ImFontConfig icon_font_cfg;
		//icon_font_cfg.MergeMode = true;
		icon_font_cfg.PixelSnapH = true;
		icon_font_cfg.OversampleH = 7;
		icon_font_cfg.GlyphMinAdvanceX = window_icon_font_size;
		static ImVector<ImWchar> ranges;
		ImFontGlyphRangesBuilder builder;
		builder.AddChar(0xE8BB); // ChromeClose
		builder.AddChar(0xE921); // ChromeMinimize
		builder.AddChar(0xE922); // ChromeMaximize
		builder.AddChar(0xE923); // ChromeRestore
		builder.BuildRanges(&ranges);              
		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segmdl2.ttf", window_icon_font_size, &icon_font_cfg, ranges.Data);

		io.Fonts->AddFontFromMemoryCompressedBase85TTF(font_cousine_compressed_data_base85, 12.f);
		//io.Fonts->AddFontFromFileTTF("assets/fonts/consola.ttf", 12);

		io.Fonts->Build();
	}

}
