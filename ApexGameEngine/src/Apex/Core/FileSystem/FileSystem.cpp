#include "apex_pch.h"
#include "FileSystem.h"

#include "MountPoint.h"

#include <regex>

#define STR(_x_) #_x_
#define EXSTR(_x_) STR(_x_)

#ifndef APEX_INSTALL_LOCATION
#error Install location not specified
#endif

namespace Apex {
	
	using MountPointPtr = Unique<VFS::IMountPoint>;
	

	static constexpr const char dirRegexStr[] = R"(([a-zA-Z0-9_-]+)(/[a-zA-Z0-9_-]+)*/?)";
	static const std::regex dirRegex(dirRegexStr);
	static constexpr const char fileRegexStr[] = R"(([a-zA-Z0-9_-]+/)*([a-zA-Z0-9_-])+\.[a-z0-9]+)";
	static const std::regex fileRegex(fileRegexStr);
	
	static Unique<VFS::IMountPoint> CreateMountPoint(const fs::path& phyPath)
	{
		if (!fs::exists(phyPath)) {
			APEX_CORE_ERROR("Path `{}` does not exist!", phyPath);
			return nullptr;
		}
		
		if (fs::is_directory(phyPath))
			return CreateUnique<VFS::PhysicalMountPoint>(phyPath);

		if (phyPath.extension() == ".apkg")
			return nullptr; // TODO: Open package file and return mount point of package root

		return nullptr;
	}
	
	struct VFSData
	{
		MountPointPtr Root;
		bool RootMounted = false;

		// Deprecated
		std::unordered_map<std::string, std::vector<MountPointPtr>> MountPoints;
	};
	
	static VFSData *s_Data = nullptr;
	
	void FileSystem::Init()
	{
		s_Data = new VFSData();
		s_Data->Root = CreateUnique<VFS::PhysicalMountPoint>();
		Mount("internal_assets", APEX_INSTALL_LOCATION "/assets");
	}

	void FileSystem::Shutdown()
	{
		delete s_Data;
	}

	bool FileSystem::MountRoot(const fs::path& _physicalPath)
	{
		fs::path physicalPath = fs::absolute(_physicalPath);
		auto mntPointPtr = CreateMountPoint(physicalPath);
		if (!mntPointPtr)
			return false;

		if (!s_Data->Root)
			s_Data->Root = std::move(mntPointPtr);
		else
			s_Data->Root->m_PhysicalPath = mntPointPtr->m_PhysicalPath;

		s_Data->Root->OnMount();
		s_Data->RootMounted = true;
		APEX_CORE_INFO("Mounted \"{0}\" @ \"/\"", physicalPath.string());

		// Recurse through the directory to cache subdirectories and mount packages
		for (auto& dirEntry : fs::recursive_directory_iterator(physicalPath)) {
			if (dirEntry.is_directory()) {
				APEX_CORE_DEBUG("sub_dir: {}", dirEntry.path());
				// If caching is enabled then store subdirectory metadata
			}
			else if (dirEntry.is_regular_file() && dirEntry.path().extension() == ".apkg") {
				APEX_CORE_DEBUG("pkg: {}", dirEntry.path().filename());
				// Mount package as subdirectory
			}
		}

		return true;
	}

	bool FileSystem::Mount(const fs::path& virtualPath, const fs::path& _physicalPath)
	{
		// APEX_CORE_ASSERT(s_Data->RootMounted, "Cannot mount path. Root is not mounted.");
		APEX_CORE_ASSERT(std::regex_match(virtualPath.string(), dirRegex), fmt::format("Incorrect mount path format -> \"{0}\"", virtualPath));

		fs::path physicalPath = fs::absolute(_physicalPath);
		auto mntPtPtr = CreateMountPoint(physicalPath);
		if (!mntPtPtr)
			return false;
		
		VFS::IMountPoint* curMountPoint = s_Data->Root.get();
		fs::path curPath("");
		for (auto pathIt = virtualPath.begin(); pathIt != --virtualPath.end(); ++pathIt) {
			curPath /= *pathIt;
			if (curMountPoint->m_SubMountPoints.count(pathIt->string()) == 0) {
				APEX_CORE_ERROR("Cannot mount `{0}`. Mount point at `{1}` not found", virtualPath, curPath);
				return false;
			}
			else {
				curMountPoint = curMountPoint->m_SubMountPoints.find(pathIt->string())->second.get();
			}
		}

		if (curMountPoint->Contains(virtualPath.filename().string())) {
			APEX_CORE_ERROR("Directory {0} already exists!", virtualPath);
			return false;
		}
		if (curMountPoint->m_SubMountPoints.count(virtualPath.filename().string()) > 0) {
			APEX_CORE_ERROR("Mount point {0} already exists!", virtualPath);
			return false;
		}

		mntPtPtr->OnMount();
		curMountPoint->m_SubMountPoints.emplace(virtualPath.filename().string(), std::move(mntPtPtr));
		APEX_CORE_INFO("Mounted \"{0}\" @ \"{1}\"", physicalPath.string(), virtualPath);

		// Recurse through the directory to cache subdirectories and mount packages
		for (auto& dirEntry : fs::recursive_directory_iterator(physicalPath)) {
			if (dirEntry.is_directory()) {
				APEX_CORE_DEBUG("sub_dir: {}", dirEntry.path());
				// If caching is enabled then store subdirectory metadata
			}
			else if (dirEntry.is_regular_file() && dirEntry.path().extension() == ".apkg") {
				APEX_CORE_DEBUG("pkg: {}", dirEntry.path().filename());
				// Mount package as subdirectory
			}
		}

		return true;
	}

	std::pair<VFS::IMountPoint*, fs::path> FileSystem::GetInnerMostMountPoint(const fs::path& filePath)
	{
		VFS::IMountPoint* curMountPoint = s_Data->Root.get();
		fs::path curPath("");
		if (std::regex_match(filePath.string(), fileRegex) && !filePath.is_absolute()) {
			for (auto pathIt = filePath.begin(); pathIt != filePath.end(); pathIt++) {
				if (curMountPoint->m_SubMountPoints.count(pathIt->string()) == 0) {
					//APEX_CORE_WARN("Mount point `{0}` not found", curPath);
					APEX_CORE_WARN("filePath: {0} | curPath: {1}", filePath, curPath);
					APEX_CORE_WARN("relative path: {0}", filePath.lexically_relative(curPath));
					break;
				}
				else {
					curMountPoint = curMountPoint->m_SubMountPoints.find(pathIt->string())->second.get();
				}
				curPath /= *pathIt;
			}
		}
		return { curMountPoint, curPath };
	}

	Ref<VFS::IFile> FileSystem::GetFileIfExists(const fs::path& filePath)
	{
		// fs::path filePath(filePathStr);
		if (filePath.is_absolute() && fs::exists(filePath))
			return Apex::CreateRef<VFS::PhysicalFile>(filePath);

		Ref<VFS::IFile> returnFile(nullptr);

		auto [mountPoint, mountPointPath] = GetInnerMostMountPoint(filePath);
		auto relativePath = filePath.lexically_relative(mountPointPath);
		returnFile = mountPoint->GetFileIfExists(relativePath);

		if (!returnFile)
			APEX_CORE_ERROR("File `{}` does not exist!", filePath);
		
		return returnFile;
	}

	Ref<File> FileSystem::MakeFile(const fs::path& filePath, bool makeParents)
	{
		if (filePath.is_absolute() && fs::exists(filePath.parent_path()))
			return Apex::CreateRef<VFS::PhysicalFile>(filePath);

		Ref<VFS::IFile> returnFile(nullptr);

		auto [mountPoint, mountPointPath] = GetInnerMostMountPoint(filePath);
		if (makeParents)
			mountPoint->MakeDirectories(filePath.parent_path().string());
		auto relativePath = filePath.lexically_relative(mountPointPath);
		returnFile = mountPoint->MakeFile(relativePath);

		return returnFile;
	}

	void FileSystem::VisitDirectory(const fs::path& dirPath, FileSystem::VisitorFn func)
	{
		auto [mountPoint, mountPointPath] = GetInnerMostMountPoint(dirPath);
		auto relativePath = dirPath.lexically_relative(mountPointPath);
		if (mountPoint->Contains(relativePath)) {
			mountPoint->Visit(func, relativePath);
		}
		else {
			APEX_CORE_ERROR("Directory `{0}` does not exist.", dirPath);
		}
	}

	void FileSystem::VisitDirectoryRecursive(const fs::path& dirPath, FileSystem::VisitorFn func)
	{
		auto [mountPoint, mountPointPath] = GetInnerMostMountPoint(dirPath);
		auto relativePath = dirPath.lexically_relative(mountPointPath);
		if (mountPoint->Contains(relativePath)) {
			mountPoint->VisitRecursive(func, relativePath);
		}
		else {
			APEX_CORE_ERROR("Directory `{0}` does not exist.", dirPath);
		}
	}

#if 0
	// Deprecated
	bool FileSystem::Mount(const std::string& virtualPathStr, const std::string& physicalPathStr)
	{
		APEX_CORE_ASSERT(std::regex_match(virtualPathStr, dirRegex), "Incorrect mount path format -> \"" + virtualPathStr + "\"");

		fs::path phyPath = fs::absolute(physicalPathStr);
		auto mntPtPtr = CreateMountPoint(phyPath);
		if (!mntPtPtr)
			return false;

		if (s_Data->MountPoints.count(virtualPathStr) == 0)
			s_Data->MountPoints.emplace(virtualPathStr, std::vector<MountPointPtr>());
		s_Data->MountPoints[virtualPathStr].push_back(std::move(mntPtPtr));

		s_Data->MountPoints[virtualPathStr].back()->OnMount();

		APEX_CORE_DEBUG("Mounted \"{0}\" @ \"{1}\"", phyPath.string(), virtualPathStr);

		return true;
	}

	Ref<VFS::IFile> FileSystem::GetFileIfExists(const std::string& filePathStr)
	{
		if (fs::exists(filePathStr))
			return Apex::CreateRef<VFS::PhysicalFile>(filePathStr);

		if (std::regex_match(filePathStr, fileRegex) && !fs::path(filePathStr).is_absolute()) {
			std::istringstream fpss(filePathStr);
			char rootSlash;
			fpss >> rootSlash;
			std::string rootDir;
			std::getline(fpss, rootDir, '/');
			std::string relativePath;
			std::getline(fpss, relativePath);
			rootDir = "/" + rootDir;
			if (s_Data->MountPoints.count(rootDir) == 0) {
				APEX_CORE_ERROR("`{}` is not a valid mount point!", rootDir);
				return nullptr;
			}
			for (auto& mntPtPtr : s_Data->MountPoints[rootDir]) 
				return mntPtPtr->GetFileIfExists(relativePath);
		}
		APEX_CORE_ERROR("File `{}` does not exist!", filePathStr);
		return nullptr;
	}

	Ref<VFS::IFile> FileSystem::MakeFile(const std::string& filePathStr)
	{
		fs::path filepath(filePathStr);
		if (fs::exists(filepath.parent_path()))
			return Apex::CreateRef<VFS::PhysicalFile>(filePathStr);

		if (std::regex_match(filePathStr, fileRegex) && !fs::path(filePathStr).is_absolute()) {
			std::istringstream fpss(filePathStr);
			char rootSlash;
			fpss >> rootSlash;
			std::string rootDir;
			std::getline(fpss, rootDir, '/');
			std::string relativePath;
			std::getline(fpss, relativePath);
			rootDir = "/" + rootDir;
			if (s_Data->MountPoints.count(rootDir) == 0) {
				APEX_CORE_ERROR("`{}` is not a valid mount point!", rootDir);
				return nullptr;
			}
			for (auto& mntPtPtr : s_Data->MountPoints[rootDir])
				return mntPtPtr->MakeFile(relativePath);
		}

		return Ref<VFS::IFile>();
	}
#endif

}
