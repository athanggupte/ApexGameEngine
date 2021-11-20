#include "apex_pch.h" 
#include "Utils.h"

#define STBI_NO_JPEG
#include <filesystem>
#include <stb_image.h>

namespace Apex {

	Apex::PixelData::~PixelData()
	{
		if (pixels)
			stbi_image_free(pixels);
	}

	namespace Utils {

		std::string GetFilename(const std::string& filepath)
		{
			auto lastSlash = filepath.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto lastDot = filepath.rfind('.');
			auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
			return filepath.substr(lastSlash, count);
		}

		ImageData LoadImage_internal(const std::filesystem::path& file, int32_t targetChannels)
		{
			ImageData imageData;
			imageData.pixelData = CreateRef<PixelData>();
			imageData.pixelData->pixels = stbi_load(file.string().c_str(), &imageData.width, &imageData.height, &imageData.channels, targetChannels);
			if (!imageData.pixelData->pixels)
				APEX_CORE_ERROR("Could not load image! [stbi]: {}", stbi_failure_reason());
			
			return imageData;
		}
	}

	namespace std140 {

		uint32_t GetNextPosition(uint32_t currentPosition, uint32_t currentSize, DataAlignment alignment)
		{
			uint32_t nextPosition = (currentPosition + currentSize + (alignment - 1)) & ~(alignment - 1);
			return nextPosition;
		}

	}

}
