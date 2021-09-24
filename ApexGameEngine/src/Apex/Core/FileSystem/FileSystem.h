#pragma once

#include <filesystem>
namespace fs = std::filesystem;

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
			fs::path path;

			bool IsDirectory() { return (type == DIRECTORY) || (type == PACKAGE); }
			bool IsFile() { return type == FILE; }
		};

		using VisitorFn = std::function<void(const Metadata&)>;

	public:
		static void Init();
		static void Shutdown();
		static bool MountRoot(const std::string& physicalPath);
		static bool Mount(const std::string& virtualPath, const std::string& physicalPath);
		static Ref<File> GetFileIfExists(const std::string& filePath);
		static Ref<File> MakeFile(const std::string& filePath, bool makeParents = false);
		static void VisitDirectory(const std::string& path, VisitorFn func);
		static void VisitDirectoryRecursive(const std::string& path, VisitorFn func);
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
