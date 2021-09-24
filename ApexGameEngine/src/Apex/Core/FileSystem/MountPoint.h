#pragma once

#include <set>
#include <fstream>

#include "FileSystem.h"

namespace Apex {
	
	// Forward Declarations
	class FileSystem;

	namespace VFS {

		class IMountPoint
		{
		public:
			IMountPoint(const fs::path& physicalPath = fs::path());
			virtual ~IMountPoint() = default;
			
			void OnMount();

			virtual bool Contains(const fs::path& filePath) const = 0;
			virtual Ref<IFile> GetFileIfExists(const fs::path& filePath) const = 0;
			virtual Ref<IFile> MakeFile(const fs::path& filePath) const = 0;
			virtual void MakeDirectories(const fs::path& dirPath) const = 0;

		protected:
			virtual void Visit(FileSystem::VisitorFn func, const fs::path& targetPath) const = 0;
			virtual void VisitRecursive(FileSystem::VisitorFn func, const fs::path& targetPath) const = 0;
			virtual void OnMountImpl() = 0;

		protected:
			std::map<std::string, Scope<IMountPoint>> m_SubMountPoints;
			fs::path m_PhysicalPath;
			bool m_IsMounted = false;

			friend class Apex::FileSystem;
		};
		
		class PhysicalMountPoint : public IMountPoint
		{
		public:
			PhysicalMountPoint(const fs::path& physicalPath = fs::path());
			~PhysicalMountPoint() = default;
			
			virtual void OnMountImpl() override;
			virtual bool Contains(const fs::path& filePath) const override;
			virtual Ref<IFile> GetFileIfExists(const fs::path& filePath) const override;
			virtual Ref<IFile> MakeFile(const fs::path& filePath) const override;
			virtual void MakeDirectories(const fs::path& dirPath) const override;

		protected:
			virtual void Visit(FileSystem::VisitorFn func, const fs::path& targetPath) const override;
			virtual void VisitRecursive(FileSystem::VisitorFn func, const fs::path& targetPath) const override;
			void CacheFilenames();
			
		private:
			std::set<fs::path> m_Filenames; // Cache the files in the directory
			bool m_IsCached = false;
		};
		
	}
	
}
