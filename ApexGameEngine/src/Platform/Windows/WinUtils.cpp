#include "apex_pch.h"

#ifdef APEX_PLATFORM_WINDOWS

#pragma comment (lib, "rpcrt4.lib")
#include <Rpc.h>

#ifdef WIN32_LEAN_AND_MEAN
#define UNDEF_WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

#pragma comment (lib, "Comdlg32.lib")
#include <Windows.h>
#include <commdlg.h>

namespace Apex {

	Apex::GUID GenerateGUID()
	{
		guid_t guid;
		(void)UuidCreate((UUID*)guid.data());
		return Apex::GUID(guid);
	}

	namespace Utils {
		
		std::string OpenFileDialog(const std::string& path)
		{
			OPENFILENAMEA ofn;       // common dialog box structure
			char szFile[260];       // buffer for file name
			// HWND hwnd;              // owner window
			HANDLE hf;              // file handle

			// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			// ofn.hwndOwner = hwnd;
			ofn.lpstrFile = szFile;
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = path.c_str();
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			// Display the Open dialog box. 

			if (GetOpenFileNameA(&ofn) == TRUE)
				return std::string{ ofn.lpstrFile };
		}

	}

}

#ifdef UNDEF_WIN32_LEAN_AND_MEAN
#undef UNDEF_WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#endif // APEX_PLATFORM_WINDOWS