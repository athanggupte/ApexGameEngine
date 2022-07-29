#pragma once

#include "Util/HashFNV1a.h"

namespace Apex {

	// using StringHandle = uint64_t;

	struct StringHandle;

	class APEX_API Strings
	{
	public:
		static StringHandle InternString(uint64_t hash, std::string&& str);
		static StringHandle Add(uint64_t hash, std::string&& str);
		static std::string_view Get(uint64_t hash);
		static bool Has(uint64_t hash);
	};


	struct APEX_API StringHandle
	{
		uint64_t hash;

		explicit operator uint64_t() const;
		[[nodiscard]] std::string_view str() const;
	};

}


#define HASH(_str_) Apex::Strings::InternString(hash_64_fnv1a_const(_str_), _str_)