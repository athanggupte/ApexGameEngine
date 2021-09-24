#include "apex_pch.h" 
#include "Utils.h"

#define STBI_NO_JPEG
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

		ImageData LoadImage(const Ref<File>& file, int32_t targetChannels)
		{
			ImageData imageData;

			if (file && file->OpenRead()) {
				auto size = file->Size();
				uint8_t* data = new uint8_t[size];
				file->Read(data, size);
				file->Close();

				// imageData.pixels = stbi_load_from_memory(data, size, &imageData.width, &imageData.height, &imageData.channels, targetChannels);
				imageData.pixelData = CreateRef<PixelData>();
				imageData.pixelData->pixels = stbi_load(file->GetPhysicalPath().c_str(), &imageData.width, &imageData.height, &imageData.channels, targetChannels);
				if (!imageData.pixelData->pixels)
					APEX_CORE_ERROR("Could not load image!");
				
				APEX_CORE_ERROR("{}", stbi_failure_reason());
				
				delete[] data;
			}
			
			return imageData;
		}
	}

	namespace std140 {

		uint32_t GetNextPosition(uint32_t currentPosition, uint32_t currentSize, std140::DataAlignment alignment)
		{
			uint32_t nextPosition = (currentPosition + currentSize + (alignment - 1)) & ~(alignment - 1);
			return nextPosition;
		}

	}

}
