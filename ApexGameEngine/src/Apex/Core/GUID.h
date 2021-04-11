#pragma once

#include <array>
#include <string>

namespace Apex {
	
	using guid_t = std::array<uint8_t, 16>;
	
	class GUID
	{
	public:
		GUID();
		GUID(const GUID&);
		GUID(GUID&&);
		~GUID() = default;
		
		std::string GetString() const;
		bool operator == (const GUID&) const;
		operator bool() const;

		void operator = (const GUID&);
		
	private:
		guid_t m_Guid;
		
		GUID(guid_t guid);
		
		friend GUID GenerateGUID();
		friend struct std::hash<GUID>;
	};
	
	GUID GenerateGUID(); // Implemented in LinuxGUID.cpp, WindowsGUID.cpp, ...
	
}

	
template<>
struct std::hash<Apex::GUID>
{
	typedef Apex::GUID argument_type;
	typedef size_t result_type;
	
	result_type operator () (const argument_type& key) const
	{
		size_t seed = 0;
		for(auto i=0; i <= 16; ++i)
		{
			seed ^= static_cast<size_t>(key.m_Guid[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}

		return seed;
	}
};
