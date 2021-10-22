#include "apex_pch.h"
#include "MountPoint.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace Apex::VFS {
	
	IMountPoint::IMountPoint(const fs::path& physicalPath)
		: m_PhysicalPath(physicalPath)
	{
	}

	void IMountPoint::OnMount()
	{
		m_IsMounted = true;
		this->OnMountImpl();
	}

	PhysicalMountPoint::PhysicalMountPoint(const fs::path& physicalPath)
		: IMountPoint(physicalPath)
	{
	}
	
	void PhysicalMountPoint::OnMountImpl()
	{
		//CacheFilenames();
		//std::stringstream ss;
		//ss << "Files in `" << m_PhysicalPath << "` :";
		//for (auto& f : m_Filenames)
		//	ss << "\n\t\t" << f;
		//APEX_CORE_DEBUG(ss.str());
	}
	
	bool PhysicalMountPoint::Contains(const fs::path& filePath) const
	{
		if (m_PhysicalPath.native().empty())
			return false;

		if (m_IsCached)
			return (bool)m_Filenames.count(filePath);
		else
			return fs::exists(m_PhysicalPath / filePath);
	}
	
	Apex::Ref<IFile> PhysicalMountPoint::GetFileIfExists(const fs::path& filePath) const
	{
		APEX_CORE_ASSERT(!m_PhysicalPath.native().empty(), "Physical path not loaded!");

		auto physicalPath = m_PhysicalPath / filePath;
		if (fs::exists(physicalPath) && fs::is_regular_file(physicalPath))
			return Apex::CreateRef<PhysicalFile>(physicalPath.string());
		else
			return nullptr;
	}

	Apex::Ref<IFile> PhysicalMountPoint::MakeFile(const fs::path& filePath) const
	{
		APEX_CORE_ASSERT(!m_PhysicalPath.native().empty(), "Physical path not loaded!");

		auto physicalPath = m_PhysicalPath / filePath;
		auto parentPath = physicalPath.parent_path();
		if (!fs::exists(parentPath) || !fs::is_directory(parentPath)) {
			APEX_CORE_ERROR("Could not create file `{0}`. Parent directory does not exist.", filePath);
			return nullptr;
		}

		return Apex::CreateRef<PhysicalFile>(physicalPath.string());
	}

	void PhysicalMountPoint::MakeDirectories(const fs::path& dirPath) const
	{
		APEX_CORE_ASSERT(!m_PhysicalPath.native().empty(), "Physical path not loaded!");

		auto phyPath = m_PhysicalPath / dirPath;
		if (!fs::exists(phyPath))
			fs::create_directories(phyPath);
	}
	
	void PhysicalMountPoint::Visit(FileSystem::VisitorFn func, const fs::path& targetPath) const
	{
		APEX_CORE_ASSERT(!m_PhysicalPath.native().empty(), "Physical path not loaded!");

		auto physicalTargetPath = m_PhysicalPath / targetPath;
		for (auto dirEntry : fs::directory_iterator(physicalTargetPath)) {
			FileSystem::Metadata metadata;
			metadata.size = dirEntry.is_regular_file() ? dirEntry.file_size() : 0;
			metadata.type = dirEntry.is_directory() ? FileSystem::Type::DIRECTORY :
				dirEntry.is_regular_file() ?
				(dirEntry.path().extension() == ".apkg" ? FileSystem::Type::PACKAGE : FileSystem::Type::FILE) :
				FileSystem::Type::OTHER;
			metadata.path = fs::relative(dirEntry.path(), m_PhysicalPath);

			func(metadata);
		}
	}

	void PhysicalMountPoint::VisitRecursive(FileSystem::VisitorFn func, const fs::path& targetPath) const
	{
		APEX_CORE_ASSERT(!m_PhysicalPath.native().empty(), "Physical path not loaded!");

		auto physicalTargetPath = m_PhysicalPath / targetPath;
		for (auto dirEntry : fs::recursive_directory_iterator(physicalTargetPath)) {
			FileSystem::Metadata metadata;
			metadata.size = dirEntry.is_regular_file() ? dirEntry.file_size() : 0;
			metadata.type = dirEntry.is_directory() ? FileSystem::Type::DIRECTORY :
				dirEntry.is_regular_file() ?
				(dirEntry.path().extension() == ".apkg" ? FileSystem::Type::PACKAGE : FileSystem::Type::FILE) :
				FileSystem::Type::OTHER;
			metadata.path = fs::relative(dirEntry.path(), m_PhysicalPath);

			func(metadata);
		}
	}

	void PhysicalMountPoint::CacheFilenames()
	{
		APEX_CORE_ASSERT(!m_PhysicalPath.native().empty(), "Physical path not loaded!");

		for (auto fileEntry : fs::recursive_directory_iterator(m_PhysicalPath)) {
			if (fileEntry.is_regular_file()) {
				fs::path path;
				/*fs::path dirPath(m_PhysicalPath);
				auto dirPathIt = dirPath.begin();
				for (auto fpit = fileEntry.path().begin(); fpit != fileEntry.path().end(); ++fpit) {
					if (dirPathIt != dirPath.end() && *fpit == *dirPathIt) {
						++dirPathIt;
						continue;
					}
					path = path / *fpit;
				}*/
				path = fs::relative(fileEntry.path(), m_PhysicalPath);
				m_Filenames.insert(path);
			}
		}
		m_IsCached = true;
	}
	
}
