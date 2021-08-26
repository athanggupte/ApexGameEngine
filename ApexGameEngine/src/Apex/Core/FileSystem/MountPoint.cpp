#include "apex_pch.h"
#include "MountPoint.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace Apex::VFS {
	
	PhysicalMountPoint::PhysicalMountPoint(const std::string& physicalPath)
		: m_PhysicalPath(physicalPath)
	{
	}
	
	void PhysicalMountPoint::OnMount()
	{
		//CacheFilenames();
		//std::stringstream ss;
		//ss << "Files in `" << m_PhysicalPath << "` :";
		//for (auto& f : m_Filenames)
		//	ss << "\n\t\t" << f;
		//APEX_CORE_DEBUG(ss.str());
	}
	
	bool PhysicalMountPoint::HasFile(const std::string& filePath) const
	{
		if (m_IsCached)
			return (bool)m_Filenames.count(filePath);
		else
			return fs::exists(m_PhysicalPath + "/" + filePath);
	}
	
	Apex::Ref<IFile> PhysicalMountPoint::GetFile(const std::string& filePath) const
	{
		if (HasFile(filePath))
			return Apex::CreateRef<PhysicalFile>(m_PhysicalPath + "/" + filePath);
		else
			return nullptr;
	}
	
	void PhysicalMountPoint::CacheFilenames()
	{
		for (auto fileEntry : fs::recursive_directory_iterator(m_PhysicalPath)) {
			if (fileEntry.is_regular_file()) {
				fs::path path;
				fs::path dirPath(m_PhysicalPath);
				auto dirPathIt = dirPath.begin();
				for (auto fpit = fileEntry.path().begin(); fpit != fileEntry.path().end(); ++fpit) {
					if (dirPathIt != dirPath.end() && *fpit == *dirPathIt) {
						++dirPathIt;
						continue;
					}
					path = path / *fpit;
				}
				m_Filenames.insert(path.string());
			}
		}
		m_IsCached = true;
	}
	
}
