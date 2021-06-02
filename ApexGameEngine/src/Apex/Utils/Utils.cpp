#include "apex_pch.h" 
#include "Utils.h"

namespace Apex {
	
	std::string GetFilename(const std::string& filepath)
	{
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		return filepath.substr(lastSlash, count);
	}
	
	namespace std140 {
	
		uint32_t GetNextPosition(uint32_t currentPosition, uint32_t currentSize, std140::DataAlignment alignment)
		{
			uint32_t nextPosition = (currentPosition + currentSize + (alignment - 1)) & ~(alignment - 1);
			return nextPosition;
		}
		
	}
}
