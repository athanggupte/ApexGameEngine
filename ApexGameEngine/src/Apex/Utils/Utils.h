#pragma once

namespace Apex {
	
	namespace Utils {
	
		std::string GetFilename(const std::string& filepath);
		
		struct FileDialogFilters
		{
			enum FileType
			{
				Any,
				
				// Project related file types
				ApexProject,
				ApexScene,
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
		
		std::string OpenFileDialog(const std::string& path = ".");
		
	}
	
}
