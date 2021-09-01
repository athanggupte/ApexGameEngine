#include "apex_pch.h"

#ifdef APEX_PLATFORM_LINUX

#include "Apex/Core/GUID.h"
#include <uuid/uuid.h>

namespace Apex {

	GUID GenerateGUID()
	{
		guid_t guid;
		uuid_generate(guid.data());
		return GUID(guid);
	}

	namespace Utils {

		std::string OpenFileDialog(const std::string& path)
		{
			char* result = nullptr;

		#if LINUX_DE_GNOME
			#error Desktop Environment not supported yet!
		#elif LINUX_DE_KDE
			#warning Using KDialog for file open / save
				// Use KDialog
				std::string cmd = "kdialog --getopenfilename " + path /* + filters */;

			FILE* stream = popen(cmd.c_str(), "r");
			char buffer[256] = { 0 };

			if (stream)
				result = fgets(buffer, 256, stream);

			pclose(stream);
		#elif LINUX_DE_LXQT
			// Use Qt
			#error Desktop Environment not supported yet!
		#else
			// Use Qt
			#error Desktop Environment not supported yet!
		#endif

			if (result) {
				std::string filename{ result };
				if (filename.back() == '\n')
					filename.back() = 0;
				return filename;
			}
			else return "";
		}
	}
	
}

#endif // APEX_PLATFORM_LINUX