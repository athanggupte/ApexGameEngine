#include "apex_pch.h" 
#include "Utils.h"

#include <stb_image.h>

namespace Apex {
	
	Apex::ImageData::~ImageData()
	{
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
	
	ImageData LoadImage(const std::string& path, int32_t targetChannels)
	{
		ImageData imageData;
		imageData.pixels = stbi_load(path.c_str(), &imageData.width, &imageData.height, &imageData.channels, targetChannels);
		return imageData;
	}
	
}
}
