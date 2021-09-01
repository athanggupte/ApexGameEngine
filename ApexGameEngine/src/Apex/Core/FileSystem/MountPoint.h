#pragma once

#include <set>
#include <fstream>

#include "FileHandle.h"

namespace Apex {
	
	namespace VFS {
		
		class IMountPoint
		{
		public:
			virtual ~IMountPoint() = default;
			
			virtual void OnMount() = 0;
			virtual bool HasFile(const std::string& filePath) const = 0;
			virtual Ref<IFile> GetFileIfExists(const std::string& filePath) const = 0;
			virtual Ref<IFile> MakeFile(const std::string& filePath) const = 0;
		};
		
		class PhysicalMountPoint : public IMountPoint
		{
		public:
			PhysicalMountPoint(const std::string& physicalPath);
			~PhysicalMountPoint() = default;
			
			virtual void OnMount() override;
			virtual bool HasFile(const std::string& filePath) const override;
			virtual Ref<IFile> GetFileIfExists(const std::string& filePath) const override;
			virtual Ref<IFile> MakeFile(const std::string& filePath) const override;

		protected:
			void CacheFilenames();
			
		private:
			std::string m_PhysicalPath;
			std::set<std::string> m_Filenames; // Cache the files in the directory
			bool m_IsCached = false;
		};
		
	}
	
}
