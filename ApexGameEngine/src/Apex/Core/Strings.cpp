#include "apex_pch.h"
#include "Strings.h"

#include "Apex/Utils/CustomDataStructures.h"

namespace Apex {

	static StringStorage s_Storage;

	StringHandle Strings::Add(uint64_t hash, std::string&& str)
	{
		return { s_Storage.Add(hash, std::move(str)) };
	}

	std::string_view Strings::Get(uint64_t hash)
	{
		return s_Storage.Get(hash);
	}

	bool Strings::Has(uint64_t hash)
	{
		return s_Storage.Has(hash);
	}

	StringHandle Strings::InternString(uint64_t hash, std::string&& str)
	{
		if (s_Storage.Has(hash))
			return { hash };
		return { s_Storage.Add(hash, std::move(str)) };
	}

}