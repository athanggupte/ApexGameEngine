#include "apex_pch.h"

#include <filesystem>

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
		
		std::string OpenFileDialog(const std::filesystem::path& path)
		{
			OPENFILENAMEA ofn;       // common dialog box structure
			char szFile[MAX_PATH];       // buffer for file name
			// HWND hwnd;              // owner window
			HANDLE hf;              // file handle

			CopyMemory(szFile, path.filename().string().c_str(), path.filename().string().size());
			szFile[path.filename().string().size()] = '\0';

			// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			// ofn.hwndOwner = hwnd;
			ofn.lpstrFile = szFile;
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			// ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "All\0*.*\0Plain Text\0*.txt\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = path.empty()
				                      ? nullptr
				                      : (is_directory(path)
					                         ? path.string().c_str()
					                         : (path.has_parent_path()
						                            ? path.parent_path().string().c_str()
						                            : nullptr));
			ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			// Display the Open dialog box. 

			if (GetOpenFileNameA(&ofn) == TRUE)
				return std::string{ ofn.lpstrFile };
			
			return {};
		}

		std::string SaveFileDialog(const std::filesystem::path& path)
		{
			OPENFILENAMEA ofn;       // common dialog box structure
			char szFile[MAX_PATH];       // buffer for file name
			// HWND hwnd;              // owner window
			HANDLE hf;              // file handle

			CopyMemory(szFile, path.filename().string().c_str(), path.filename().string().size());
			szFile[path.filename().string().size()] = '\0';

			// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			// ofn.hwndOwner = hwnd;
			ofn.lpstrFile = szFile;
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			// ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "All\0*.*\0Plain Text\0*.txt\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = path.empty()
				                      ? nullptr
				                      : (is_directory(path)
					                         ? path.string().c_str()
					                         : (path.has_parent_path()
						                            ? path.parent_path().string().c_str()
						                            : nullptr));
			ofn.lpstrDefExt = "txt";
			ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

			// Display the Open dialog box. 

			if (GetSaveFileNameA(&ofn) == TRUE)
				return std::string{ ofn.lpstrFile };
			
			return {};
		}

	}

}

#ifdef UNDEF_WIN32_LEAN_AND_MEAN
#undef UNDEF_WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#endif // APEX_PLATFORM_WINDOWS