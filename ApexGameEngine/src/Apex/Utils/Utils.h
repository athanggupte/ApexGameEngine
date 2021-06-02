#pragma once

namespace Apex {
	
	std::string GetFilename(const std::string& filepath);
	
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
		
		uint32_t GetNextPosition(uint32_t currentPosition, uint32_t currentSize, std140::DataAlignment alignment);
		
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
