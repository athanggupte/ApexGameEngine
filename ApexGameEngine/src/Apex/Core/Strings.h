#pragma once

#include "Apex/Utils/HashFNV1a.h"

namespace Apex {

	// using StringHandle = uint64_t;

	struct StringHandle;

	class Strings
	{
	public:
		static StringHandle InternString(uint64_t hash, std::string&& str);
		static StringHandle Add(uint64_t hash, std::string&& str);
		static std::string_view Get(uint64_t hash);
		static bool Has(uint64_t hash);
	};


	struct StringHandle
	{
		uint64_t hash;

		operator uint64_t() { return hash; }
		std::string_view str() { return Strings::Get(hash); }
		const std::string_view str() const { return Strings::Get(hash); }
	};

}


#define HASH(_str_) Apex::Strings::InternString(hash_64_fnv1a_const(_str_), _str_)