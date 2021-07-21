#include "apex_pch.h"
#include "Apex/Core/GUID.h"

#ifdef APEX_PLATFORM_LINUX

#include <uuid/uuid.h>

namespace Apex {
	
	GUID GenerateGUID()
	{
		guid_t guid;
		uuid_generate(guid.data());
		return GUID(guid);
	}
	
}

#elif defined(APEX_PLATFORM_WINDOWS)

#pragma comment (lib, "rpcrt4.lib")
#include <Rpc.h>

namespace Apex {

	Apex::GUID GenerateGUID()
	{
		guid_t guid;
		(void)UuidCreate((UUID*)guid.data());
		return Apex::GUID(guid);
	}

}

#endif // APEX_PLATFORM_LINUX
