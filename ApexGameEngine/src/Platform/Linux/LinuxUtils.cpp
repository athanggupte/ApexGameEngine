#include "apex_pch.h"

namespace Apex::Utils {
	
	std::string OpenFileDialog(const std::string& path)
	{
		char *result = nullptr;
		
	#if LINUX_DE_GNOME
		#error Desktop Environment not supported yet!
	#elif LINUX_DE_KDE
		#warning Using KDialog for file open/save
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
