#pragma once

#include "MountPoint.h"


namespace Apex {
	using File = Apex::VFS::IFile;
	
	class FileSystem
	{
	public:
		static void Init();
		static bool Mount(const std::string& virtualPath, const std::string& physicalPath);
		static Ref<File> GetFileIfExists(const std::string& filePath);
		static Ref<File> MakeFile(const std::string& filePath);
	};
	
}
