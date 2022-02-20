#pragma once

#include "CustomDataStructures.h"

#include <filesystem>

namespace Apex {
	
	struct PixelData
	{
		uint8_t* pixels = nullptr;
		~PixelData();
	};

	struct ImageData
	{
		int32_t width = 0, height = 0, channels = 0;
		Ref<PixelData> pixelData;
	};


	namespace Utils {

		std::string TrimStr(const std::string& str, const std::string& whitespace = " \t");

		ImageData LoadImage_internal(const std::filesystem::path& file, int32_t targetChannels = 0);
	
		std::string GetFilename(const std::string& filepath);
		
		struct FileDialogFilters
		{
			enum FileType
			{
				Any,

				// Project related file types
				ApexProject,
				ApexScene,
				ApexResources,
				ApexConfig,
				
				// Resources file types
				Texture,
				Model,
				Sprite,
				Sound,
				
				// Source file types
				CppSource,
				LuaSource,
				PythonSource,
			};
			
			static constexpr const char* PROJECT_FILE_TYPE[] = { "*.apx", "*.apex" };
		};
		
		struct FileDialogMode
		{
			enum : uint8_t
			{
				OPEN = 0,
				SAVE = 1
			};
			
		};
		
		std::string OpenFileDialog(const std::filesystem::path& filepath = {});
		std::string SaveFileDialog(const std::filesystem::path& filepath = {});
		
	}
	
	/*-------------------------std140 Layout-----------------------------*/
	namespace std140 {
		
		enum DataAlignment : uint32_t
		{
			// vec3 and mat3 are not allowed for maintaining consistency
			Int   = 4,
			Float = 4,
			Vec2  = 8,
			Mat2  = 8,
			Vec4  = 16,
			Mat4  = 16
		};
		
		uint32_t GetNextPosition(uint32_t currentPosition, uint32_t currentSize, DataAlignment alignment);
		
	}
	
}

template<typename T1, typename T2>
struct std::hash<std::pair<T1, T2>>
{
	size_t operator () (const std::pair<T1, T2>& pair) const
	{
		return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
	}
};

// https://stackoverflow.com/a/67059296
template<class T> T& unmove(T&& t) { return t; }

#include "base64.h"
#define BASE64(bits) base64_encode((uint8_t*)&unmove(bits), sizeof(bits), false)
#define BASE64_D(str) base64_decode(str, false)

#define TO_STRING(val) fmt::format("{}", val)
#define TO_CSTRING(val) TO_STRING(val).c_str()