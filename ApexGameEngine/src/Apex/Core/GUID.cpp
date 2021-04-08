#include "apex_pch.h" 
#include "GUID.h"

#include <cstring>

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
	static void uuid_unpack(const uuid_t in, struct uuid *uu)
	{
		const uint8_t	*ptr = in;
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

	static int uuid_compare(const uuid_t uu1, const uuid_t uu2)
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
	
	GUID::GUID(uuid_t guid)
	{
		memcpy(m_Guid, guid, 16);
	}
	
	GUID::GUID(const GUID& other)
	{
		memcpy(m_Guid, other.m_Guid, 16);
	}
	
	std::string GUID::GetString() const
	{
		// Implemented as in libuuid : uuid_unparse
		
		char str[36];
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
		
		return std::string(str);
	}
	
	bool GUID::operator == (const GUID& other) const
	{
		return uuid_compare(m_Guid, other.m_Guid) == 0;
	}
	
	GUID::operator bool() const
	{
		uint32_t *tmp = (uint32_t*)m_Guid;
		
		for (auto i=0; i<4; i++) {
			if (*tmp++) {
				return true;
			}
		}
		return false;
	}
	
}
