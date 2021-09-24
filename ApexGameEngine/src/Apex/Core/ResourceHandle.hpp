#pragma once

#include <type_traits>

namespace Apex {
	
	template<typename Resource_t> class SharedResourceHandle; // Forward declaration
	//template<typename Resource_t> class UniqueResourceHandle; // Forward declaration
	
	template<typename Resource_t>
	class SharedResourceHandle
	{
	private:
		Resource_t* m_ResourcePtr = nullptr;
		uint32_t* m_RefCount = nullptr;
		GUID* m_Guid = nullptr;
		
	public:
		template<typename Any_t> friend class SharedResourceHandle; // All other SharedResourceHandles are friends
			
		// Default constructor
		SharedResourceHandle() = default;
		SharedResourceHandle(std::nullptr_t)
			: m_ResourcePtr( nullptr ), m_RefCount( nullptr ), m_Guid( nullptr )
		{
		}
			
		// Copy Constructors ==> increase reference count of underlying reference pointer
		SharedResourceHandle(SharedResourceHandle<Resource_t>& other)
			: m_ResourcePtr( other.m_ResourcePtr ), m_RefCount( other.m_RefCount ), m_Guid( other.m_Guid )
		{
			if(m_RefCount) ++(*m_RefCount);
		}
		
		SharedResourceHandle(const SharedResourceHandle<Resource_t>& other)
			: m_ResourcePtr( other.m_ResourcePtr ), m_RefCount( other.m_RefCount ), m_Guid( other.m_Guid )
		{
			if(m_RefCount) ++(*m_RefCount);
		}
		
		template<typename DerivedResource_t,
				typename std::enable_if<std::is_convertible<DerivedResource_t*, Resource_t*>::value, bool>::type = true
				> 
		SharedResourceHandle(SharedResourceHandle<DerivedResource_t>& other)
			: m_ResourcePtr( other.m_ResourcePtr ), m_RefCount( other.m_RefCount ), m_Guid( other.m_Guid )
		{
			if(m_RefCount) ++(*m_RefCount);
		}
		
		
		template<typename DerivedResource_t,
				typename std::enable_if<std::is_convertible<DerivedResource_t*, Resource_t*>::value, bool>::type = true
				> 
		SharedResourceHandle(const SharedResourceHandle<DerivedResource_t>& other)
			: m_ResourcePtr( other.m_ResourcePtr ), m_RefCount( other.m_RefCount ), m_Guid( other.m_Guid )
		{
			if(m_RefCount) ++(*m_RefCount);
		}
		
		SharedResourceHandle(Resource_t* ptr)
			: m_ResourcePtr( ptr ), m_RefCount( new uint32_t(0) ), m_Guid( new GUID(GenerateGUID()) )
		{
			++(*m_RefCount);
		}
		
// 		template<typename DerivedResource_t,
// 				typename std::enable_if<std::is_convertible<DerivedResource_t*, Resource_t*>::value, bool>::type = true
// 				> 
// 		SharedResourceHandle(DerivedResource_t* other)
// 			: m_ResourcePtr( other ), m_RefCount( new uint32_t(0) ), m_Guid( GenerateGUID() )
// 		{
// 			if(m_RefCount) ++(*m_RefCount);
// 		}
		
		// Move Constructors ==> donot increase reference count
		SharedResourceHandle(SharedResourceHandle<Resource_t>&& other)
			: m_ResourcePtr( std::move(other.m_ResourcePtr) ), m_RefCount( std::move(other.m_RefCount) ), m_Guid( std::move(other.m_Guid) )
		{
			other.m_ResourcePtr = nullptr;
			other.m_RefCount = nullptr;
			other.m_Guid = nullptr;
		}
		
		template<typename DerivedResource_t,
				typename std::enable_if<std::is_convertible<DerivedResource_t*, Resource_t*>::value, bool>::type = true
				> 
		SharedResourceHandle(SharedResourceHandle<DerivedResource_t>&& other)
			: m_ResourcePtr( std::move(other.m_ResourcePtr) ), m_RefCount( std::move(other.m_RefCount) ), m_Guid( std::move(other.m_Guid) )
		{
			other.m_ResourcePtr = nullptr;
			other.m_RefCount = nullptr;
			other.m_Guid = nullptr;
		}
		
		// Destructor ==> deletes reference if count is 0
		~SharedResourceHandle()
		{
			if (m_RefCount && --(*m_RefCount) == 0) {
				if (m_ResourcePtr) delete m_ResourcePtr;
				delete m_RefCount;
				delete m_Guid;
			}
		}
		
		// Copy assignment operator
		SharedResourceHandle& operator = (const SharedResourceHandle& other)
		{
			if (m_ResourcePtr != other.m_ResourcePtr) {
				if (m_RefCount && --(*m_RefCount) == 0) {
					if (m_ResourcePtr) delete m_ResourcePtr;
					delete m_RefCount;
					delete m_Guid;
				}
				
				m_ResourcePtr = other.m_ResourcePtr;
				m_RefCount = other.m_RefCount;
				m_Guid = other.m_Guid;
				
				if(m_RefCount) ++(*m_RefCount);
			}
			return *this;
		}
		
		template<typename DerivedResource_t,
				typename std::enable_if<std::is_convertible<DerivedResource_t*, Resource_t*>::value, bool>::type = true
				> 
		SharedResourceHandle& operator = (const SharedResourceHandle<DerivedResource_t>& other)
		{
			if (m_ResourcePtr != other.m_ResourcePtr) {
				if (m_RefCount && --(*m_RefCount) == 0) {
					if (m_ResourcePtr) delete m_ResourcePtr;
					delete m_RefCount;
					delete m_Guid;
				}
				
				m_ResourcePtr = other.m_ResourcePtr;
				m_RefCount = other.m_RefCount;
				m_Guid = other.m_Guid;
				
				if(m_RefCount) ++(*m_RefCount);
			}
			return *this;
		}
		
		// Move assignment operator
		SharedResourceHandle& operator = (SharedResourceHandle&& other)
		{
			if (m_ResourcePtr != other.m_ResourcePtr) {
				if (m_RefCount && --(*m_RefCount) == 0) {
					if (m_ResourcePtr) delete m_ResourcePtr;
					delete m_RefCount;
					delete m_Guid;
				}
				
				m_ResourcePtr = other.m_ResourcePtr;
				m_RefCount = other.m_RefCount;
				m_Guid = other.m_Guid;
				
				other.m_ResourcePtr = nullptr;
				other.m_RefCount = nullptr;
				other.m_Guid = nullptr;
				
				if(m_RefCount) ++(*m_RefCount);
			}
			return *this;
		}
		
		template<typename DerivedResource_t,
				typename std::enable_if<std::is_convertible<DerivedResource_t*, Resource_t*>::value, bool>::type = true
				> 
		SharedResourceHandle& operator = (SharedResourceHandle<DerivedResource_t>&& other)
		{
			if (m_ResourcePtr != other.m_ResourcePtr) {
				if (m_RefCount && --(*m_RefCount) == 0) {
					if (m_ResourcePtr) delete m_ResourcePtr;
					delete m_RefCount;
					delete m_Guid;
				}
				
				m_ResourcePtr = other.m_ResourcePtr;
				m_RefCount = other.m_RefCount;
				m_Guid = other.m_Guid;
				
				other.m_ResourcePtr = nullptr;
				other.m_RefCount = nullptr;
				other.m_Guid = nullptr;
				
				if(m_RefCount) ++(*m_RefCount);
			}
			return *this;
		}
		
		// Getters
		uint32_t GetRefCount()
		{
			return m_RefCount ? *m_RefCount : 0;
		}
		
		Resource_t* Get() { return m_ResourcePtr; }
		const Resource_t* Get() const { return m_ResourcePtr; }
		
		Resource_t* operator -> () { return m_ResourcePtr; }
		Resource_t& operator * () { return *m_ResourcePtr; }
		
		const Resource_t* operator -> () const { return m_ResourcePtr; }
		const Resource_t& operator * () const { return *m_ResourcePtr; }
		
		const GUID& GetGUID() const { return *m_Guid; }
		
		// Convenience functions
		bool operator == (const SharedResourceHandle& other) const
		{
			return *m_Guid == *other.m_Guid;
		}
		
		operator bool() const { return m_ResourcePtr != 0; }
		
	};
	
}

template<typename Resource_t>
struct std::hash<Apex::SharedResourceHandle<Resource_t>>
{
	size_t operator () (const Apex::SharedResourceHandle<Resource_t>& key) {
		return std::hash<Apex::GUID>()(key.GetGUID());
	}
};
