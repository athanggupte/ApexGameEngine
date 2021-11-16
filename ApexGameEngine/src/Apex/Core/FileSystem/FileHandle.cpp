#include "apex_pch.h"
#include "FileHandle.h"

namespace Apex::VFS {
	
	PhysicalFile::PhysicalFile(const fs::path& filePath, bool binary)
		: IFile(filePath), m_IsBinary(binary)
	{
		APEX_CORE_DEBUG("Physical file loaded: {}", m_PhysicalPath);
	}
	
	PhysicalFile::~PhysicalFile()
	{
		this->Close();
	}
	
	bool PhysicalFile::OpenRead()
	{
		m_FileHandle.open(m_PhysicalPath, (m_IsBinary ? std::ios::binary | std::ios::in : std::ios::in));
		if (IsOpen()) {
			m_AccessMode = ReadMode;
			return true;
		}
		return false;
	}
	
	bool PhysicalFile::OpenWrite(bool append, bool allowRead)
	{
		auto mode = allowRead ? (std::ios::in | std::ios::out) : std::ios::out;
		m_FileHandle.open(m_PhysicalPath, (m_IsBinary ? (std::ios::binary | mode) : mode));
		if (IsOpen()) {
			m_AccessMode = allowRead ? WriteMode | ReadMode : WriteMode;
			return true;
		}
		return false;
	}
	
	uint32_t PhysicalFile::Read(void* data, uint32_t size)
	{
		APEX_CORE_ASSERT((IsOpen()), fmt::format("File {0} is not open!", m_PhysicalPath));
		APEX_CORE_ASSERT((m_AccessMode | ReadMode), fmt::format("File {0} is not in Read mode!", m_PhysicalPath));
		if (!(IsOpen() && m_AccessMode | ReadMode))
			return 0;
		auto start = m_FileHandle.tellg();
		m_FileHandle.read((char*)data, size);
		auto end = m_FileHandle.tellg();
		m_FileHandle.seekp(end);
		return (uint32_t)(end - start);
	}
	
	uint32_t PhysicalFile::Write(const void* data, uint32_t size)
	{
		APEX_CORE_ASSERT((IsOpen()), fmt::format("File {0} is not open!", m_PhysicalPath));
		APEX_CORE_ASSERT((m_AccessMode | WriteMode), fmt::format("File {0} is not in Write mode!", m_PhysicalPath));
		if (!(IsOpen() && m_AccessMode | WriteMode))
			return 0;
		auto start = m_FileHandle.tellp();
		m_FileHandle.write((char*)data, size);
		auto end = m_FileHandle.tellp();
		m_FileHandle.seekg(end);
		return (uint32_t)(end - start);
	}

	void PhysicalFile::Flush()
	{
		if (m_AccessMode | WriteMode)
			m_FileHandle.flush();
		if (m_AccessMode | ReadMode)
			m_FileHandle.sync();
	}
	
	void PhysicalFile::Close()
	{
		m_FileHandle.close();
		m_AccessMode = PhysicalFile::None;
	}
	
	bool PhysicalFile::IsOpen() const
	{
		return m_FileHandle.is_open();
	}
	
	uint32_t PhysicalFile::Size()
	{
		APEX_CORE_ASSERT((IsOpen()), fmt::format("File {0} is not open!", m_PhysicalPath));
		if (!IsOpen())
			return 0;
		m_FileHandle.seekg(0, std::ios::end);
		auto size = m_FileHandle.tellg();
		m_FileHandle.seekg(0, std::ios::beg);
		return size;
	}
	
	bool PhysicalFile::SeekPtr(const int64_t position, bool fromEnd)
	{
		APEX_CORE_ASSERT((IsOpen()), fmt::format("File {0} is not open!", m_PhysicalPath));
		if (!IsOpen())
			return false;
		
		if (m_AccessMode | ReadMode) {
			m_FileHandle.seekg(position, (fromEnd ? std::ios::beg : std::ios::end));
		}
		if (m_AccessMode | WriteMode) {
			m_FileHandle.seekp(position, (fromEnd ? std::ios::beg : std::ios::end));
		}
		return true;
	}
	
	uint32_t PhysicalFile::TellPtr()
	{
		APEX_CORE_ASSERT((IsOpen()), fmt::format("File {0} is not open!", m_PhysicalPath));
		if (!IsOpen())
			return (uint32_t)(-1);
		
		if (m_AccessMode | ReadMode) {
			return m_FileHandle.tellg();
		}
		if (m_AccessMode | WriteMode) {
			return m_FileHandle.tellp();
		}
	}
	
}
