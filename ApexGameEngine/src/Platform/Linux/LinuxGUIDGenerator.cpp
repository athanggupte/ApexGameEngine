#ifdef APEX_PLATFORM_LINUX

#include "apex_pch.h"
#include "Apex/Core/GUID.h"

#include <uuid/uuid.h>

namespace Apex {
	
	GUID GenerateGUID()
	{
		uuid_t guid;
		uuid_generate(guid);
		return GUID(guid);
	}
	
}

#endif // APEX_PLATFORM_LINUX
