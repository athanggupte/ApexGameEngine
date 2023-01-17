#pragma once

#include "FileHandle.h"

namespace Apex {
	using File = Apex::VFS::IFile;

	namespace VFS {
		class IMountPoint;
	}

	class FileSystem
	{
	public:
		enum Type
		{
			OTHER		= 0,
			DIRECTORY	= 1,
			FILE		= 2,
			PACKAGE		= 3,
		};

		struct Metadata
		{
			uint64_t size;
			Type type;
			fs::path relativePath;
			fs::path rootPath;

			bool IsDirectory() const { return (type == DIRECTORY) || (type == PACKAGE); }
			bool IsFile() const { return type == FILE; }
		};

		using VisitorFn = std::function<void(const Metadata&)>;

	public:
		static void Init();
		static void Shutdown();
		static bool MountRoot(const fs::path& _physicalPath);
		static bool Mount(const fs::path& virtualPath, const fs::path& _physicalPath);
		static Ref<File> GetFileIfExists(const fs::path& filePath);
		static Ref<File> MakeFile(const fs::path& filePath, bool makeParents = false);
		static void VisitDirectory(const fs::path& dirPath, VisitorFn func);
		static void VisitDirectoryRecursive(const fs::path& dirPath, VisitorFn func);

		static std::vector<uint8_t> ReadFromFile(const fs::path& filePath);
#if 0
		// Deprecated
		static bool Mount(const std::string& virtualPath, const std::string& physicalPath);
		static Ref<File> GetFileIfExists(const std::string& filePath);
		static Ref<File> MakeFile(const std::string& filePath);
#endif
	private:
		static std::pair<VFS::IMountPoint*, fs::path> GetInnerMostMountPoint(const fs::path& filePath);
	};
	
}
