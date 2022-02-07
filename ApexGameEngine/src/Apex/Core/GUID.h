#pragma once

#include <array>
#include <string>

namespace Apex {
	
	using guid_t = std::array<uint8_t, 16>;
	
	class GUID
	{
	public:
		GUID(const GUID&) = default;
		GUID(GUID&&);
		~GUID() = default;
		
		bool operator == (const GUID&) const;
		operator bool() const;

		void operator = (const GUID&);

		[[nodiscard]] size_t Hash() const { return m_Hash; };

	private:
		guid_t m_Guid;
		size_t m_Hash = 0;

		std::string GetString() const;
		void CalculateHash();

		GUID();
		GUID(const guid_t& guid);
		
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
		return key.Hash();
	}
};
