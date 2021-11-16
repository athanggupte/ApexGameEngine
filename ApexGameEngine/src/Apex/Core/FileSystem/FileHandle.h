#pragma once

#include <filesystem>
namespace fs = std::filesystem;

namespace Apex {
	
	namespace VFS {
		
		class IFile
		{
		public:
			IFile(const fs::path& filePath)
				: m_PhysicalPath(filePath)
			{}
			
			virtual ~IFile() = default;
			
			virtual bool OpenRead() = 0;
			virtual bool OpenWrite(bool append, bool allowRead) = 0;
			
			virtual uint32_t Read(void* data, uint32_t size) = 0;
			virtual uint32_t Write(const void* data, uint32_t size) = 0;
			
			virtual void Flush() = 0;
			virtual void Close() = 0;
			virtual bool IsOpen() const = 0;
			virtual uint32_t Size() = 0;
			virtual bool SeekPtr(const int64_t position, bool fromEnd = false) = 0;
			virtual uint32_t TellPtr() = 0;
			
			const fs::path& GetPhysicalPath() const { return m_PhysicalPath; }
			
		protected:
			// TODO: Do I really need this here??
			fs::path m_PhysicalPath;
		};
		
		class PhysicalFile : public IFile
		{
		public:
			PhysicalFile(const fs::path& filePath, bool binary = true);
			~PhysicalFile();
			
			virtual bool OpenRead() override;
			virtual bool OpenWrite(bool append, bool allowRead) override;
			
			virtual uint32_t Read(void* data, uint32_t size) override;
			virtual uint32_t Write(const void* data, uint32_t size) override;
			
			virtual void Flush() override;
			virtual void Close() override;
			virtual bool IsOpen() const override;
			virtual uint32_t Size() override;
			virtual bool SeekPtr(const int64_t position, bool fromEnd = false) override;
			virtual uint32_t TellPtr() override;
		
		private:
			bool m_IsBinary;
			enum : uint8_t {
				None = 0,
				ReadMode = 1,
				WriteMode = 2
			};
			uint8_t m_AccessMode;
			std::fstream m_FileHandle;
		};
		
	}
}
