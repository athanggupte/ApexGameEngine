#pragma once

#include "MountPoint.h"

namespace Apex {
	
	class FileSystem
	{
	public:
		static void Init();
		static bool Mount(const std::string& virtualPath, const std::string& physicalPath);
		static Ref<VFS::IFile> GetFileIfExists(const std::string& filePath);
		static Ref<VFS::IFile> MakeFile(const std::string& filePath);
	};
	
}
