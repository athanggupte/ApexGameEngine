#pragma once

#include <string>

namespace Apex {
	
	using uuid_t = uint8_t[16];
	
	class GUID
	{
	public:
		~GUID() = default;
		GUID(const GUID&);
		
		std::string GetString() const;
		bool operator == (const GUID&) const;
		operator bool() const;

	private:
		uuid_t m_Guid;
		
		GUID(uuid_t guid);
		
		friend GUID GenerateGUID();
	};
	
	GUID GenerateGUID(); // Implemented in LinuxGUID.cpp, WindowsGUID.cpp, ...
	
}
