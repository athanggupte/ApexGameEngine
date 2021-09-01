#include "apex_pch.h"
#include "VFS.h"

#include "MountPoint.h"

#include <regex>

#define STR(_x_) #_x_
#define EXSTR(_x_) STR(_x_)

#ifndef APEX_INSTALL_LOCATION
#error Install location not specified
#endif

#include <filesystem>
namespace fs = std::filesystem;

namespace Apex {
	
	using MountPointPtr = Scope<VFS::IMountPoint>;
	

	static constexpr const char dirRegexStr[] = R"((/[a-zA-Z0-9_-]+)+)";
	static const std::regex dirRegex(dirRegexStr);
	static constexpr const char fileRegexStr[] = R"(/([a-zA-Z0-9_-]+/)*([a-zA-Z0-9_-])+\.[a-z0-9]+)";
	static const std::regex fileRegex(fileRegexStr);
	
	static Scope<VFS::IMountPoint> CreateMountPoint(const fs::path& phyPath)
	{
		if (!fs::exists(phyPath)) {
			APEX_CORE_ERROR("Path `{}` does not exist!", phyPath.string());
			return nullptr;
		}
		
		if (fs::is_directory(phyPath))
			return CreateScope<VFS::PhysicalMountPoint>(phyPath.string());
		else if (phyPath.extension() == ".apkg")
			return nullptr;
	}
	
	static std::vector<std::string> GetDirectoryTree(const std::string& filePath)
	{
		std::vector<std::string> directoryTree;
		std::istringstream fpstream(filePath);
		char rootSlash;
		fpstream >> rootSlash;
		std::string temp;
		
		while (std::getline(fpstream, temp, '/'))
			directoryTree.push_back(temp);
		
		return directoryTree;
	}
	
	struct VFSData
	{
		std::unordered_map<std::string, std::vector<MountPointPtr>> MountPoints;
	};
	
	static VFSData *s_Data = nullptr;
	
	void FileSystem::Init()
	{
		s_Data = new VFSData();
		Mount("/core_assets", APEX_INSTALL_LOCATION "/assets");
	}
	
	bool FileSystem::Mount(const std::string& virtualPath, const std::string& physicalPath)
	{
		APEX_CORE_ASSERT(std::regex_match(virtualPath, dirRegex), "Incorrect mount path format -> \"" + virtualPath + "\"");
		if (s_Data->MountPoints.count(virtualPath))
			return false;
		
		fs::path phyPath =  fs::absolute(physicalPath);
		auto mntPtPtr = CreateMountPoint(phyPath);
		if (!mntPtPtr)
			return false;
		
		mntPtPtr->OnMount();
		s_Data->MountPoints.emplace(virtualPath, std::vector<MountPointPtr>());
		s_Data->MountPoints[virtualPath].push_back(std::move(mntPtPtr));
		
		APEX_CORE_DEBUG("Mounted \"{0}\" @ \"{1}\"", phyPath.string(), virtualPath);
		
		return true;
	}
	
	Ref<VFS::IFile> FileSystem::GetFileIfExists(const std::string& filePath)
	{
		// APEX_CORE_ASSERT(std::regex_match(filePath, reg), "Incorrect file path format -> \"" + filePath + "\"");
		if (fs::exists(filePath))
			return Apex::CreateRef<VFS::PhysicalFile>(filePath);

		if (std::regex_match(filePath, fileRegex) && !fs::path(filePath).is_absolute()) {
			std::istringstream fpss(filePath);
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
		APEX_CORE_ERROR("File `{}` does not exist!", filePath);
		return nullptr;
	}

	Ref<VFS::IFile> FileSystem::MakeFile(const std::string& filePath)
	{
		fs::path filepath(filePath);
		if (fs::exists(filepath.parent_path()))
			return Apex::CreateRef<VFS::PhysicalFile>(filePath);

		if (std::regex_match(filePath, fileRegex) && !fs::path(filePath).is_absolute()) {
			std::istringstream fpss(filePath);
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
	
}
