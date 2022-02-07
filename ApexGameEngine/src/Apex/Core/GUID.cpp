#include "apex_pch.h" 
#include "GUID.h"

#include <cstring>
#include <algorithm>

namespace Apex {
	
	constexpr static char const HEX_DIGITS[17] = "0123456789abcdef"; // From libuuid/unparse.c
	
	/* Taken from libuuid/uuidP.h */
	struct uuid {
		uint32_t	time_low;
		uint16_t	time_mid;
		uint16_t	time_hi_and_version;
		uint16_t	clock_seq;
		uint8_t	node[6];
	};
	
	/* Taken from libuuid/unpack.c */
	static void uuid_unpack(const guid_t in, struct uuid *uu)
	{
		const uint8_t	*ptr = in.data();
		uint32_t		tmp;

		tmp = *ptr++;
		tmp = (tmp << 8) | *ptr++;
		tmp = (tmp << 8) | *ptr++;
		tmp = (tmp << 8) | *ptr++;
		uu->time_low = tmp;

		tmp = *ptr++;
		tmp = (tmp << 8) | *ptr++;
		uu->time_mid = tmp;

		tmp = *ptr++;
		tmp = (tmp << 8) | *ptr++;
		uu->time_hi_and_version = tmp;

		tmp = *ptr++;
		tmp = (tmp << 8) | *ptr++;
		uu->clock_seq = tmp;

		memcpy(uu->node, ptr, 6);
	}
	
	/* Taken from libuuid/compare.c */
#define UUCMP(u1,u2) if (u1 != u2) return((u1 < u2) ? -1 : 1);

	static int uuid_compare(const guid_t uu1, const guid_t uu2)
	{
		struct uuid	uuid1, uuid2;

		uuid_unpack(uu1, &uuid1);
		uuid_unpack(uu2, &uuid2);

		UUCMP(uuid1.time_low, uuid2.time_low);
		UUCMP(uuid1.time_mid, uuid2.time_mid);
		UUCMP(uuid1.time_hi_and_version, uuid2.time_hi_and_version);
		UUCMP(uuid1.clock_seq, uuid2.clock_seq);
		return memcmp(uuid1.node, uuid2.node, 6);
	}
	
	GUID::GUID()
	{
 		// memset(m_Guid, 0, 16);
	}
	
	GUID::GUID(const guid_t& guid)
		: m_Guid(guid)
	{
		// memcpy(m_Guid.data(), guid.data(), 16);
		CalculateHash();
		Strings::Add(m_Hash, GetString());
	}
	
	//GUID::GUID(const GUID& other)
	//	: m_Guid(other.m_Guid), m_Hash(other.m_Hash)
	//{
	//	// memcpy(m_Guid.data(), other.m_Guid.data(), 16);
	//}
	
	GUID::GUID(GUID&& other)
		: m_Guid(other.m_Guid), m_Hash(other.m_Hash)
	{
		// memcpy(m_Guid, other.m_Guid, 16);
	}
	
	std::string GUID::GetString() const
	{
		// Implemented as in libuuid : uuid_unparse
		
		char str[37];
		char* p = str;
		
		for (int i = 0; i < 16; i++) {
			if (i == 4 || i == 6 || i == 8 || i == 10) {
				*p++ = '-';
			}
			size_t tmp = m_Guid[i];
			*p++ = HEX_DIGITS[tmp >> 4];
			*p++ = HEX_DIGITS[tmp & 15];
		}
		*p = '\0';
		
		return { str };
	}

	void GUID::CalculateHash()
	{
		size_t seed = 0;
		for(auto i=0; i < 16; ++i)
		{
			seed ^= static_cast<size_t>(m_Guid[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}

		m_Hash = seed;
	}

	bool GUID::operator == (const GUID& other) const
	{
		// return uuid_compare(m_Guid, other.m_Guid) == 0;
		const uint64_t* p_lhs = reinterpret_cast<const uint64_t*>(m_Guid.data());
		const uint64_t* p_rhs = reinterpret_cast<const uint64_t*>(other.m_Guid.data());

		bool equal = *p_lhs == *p_rhs;

		++p_lhs; ++p_rhs;
		equal |= *p_lhs == *p_rhs;

		return equal;
	}
	
	GUID::operator bool() const
	{
		uint32_t *tmp = (uint32_t*)m_Guid.data();
		
		for (auto i=0; i<4; i++) {
			if (*tmp++) {
				return true;
			}
		}
		return false;
	}
	
	void GUID::operator = (const GUID& other)
	{
		std::copy(other.m_Guid.begin(), other.m_Guid.end(), m_Guid.begin());
	}
	
}
