#include "apex_pch.h"
#include "ResourceManager.h"

namespace Apex {
	
	// Load
	void Resource::_LoadResource::operator() (const Ref<VFS::IFile>& texture)
	{
		APEX_LOG_INFO("Loading file '{}'", resource.m_SourceFile.str());
	}
	
	void Resource::_LoadResource::operator() (const Ref<Texture>& texture)
	{
		APEX_LOG_INFO("Loading texture from '{}'", resource.m_SourceFile.str());
		resource.m_Ptr = Texture2D::Create(std::string(resource.m_SourceFile.str()));
	}
	
	void Resource::_LoadResource::operator() (const Ref<Shader>& texture)
	{
		APEX_LOG_INFO("Loading shader from '{}'", resource.m_SourceFile.str());
	}

	// Reload
	void Resource::_ReloadResource::operator() (const Ref<VFS::IFile>& texture)
	{
		APEX_LOG_INFO("Reloading file '{}'", resource.m_SourceFile.str());
		resource.m_Ptr = Texture2D::Create(std::string(resource.m_SourceFile.str()));
	}

	void Resource::_ReloadResource::operator() (const Ref<Texture>& texture)
	{
		APEX_LOG_INFO("Reloading texture from '{}'", resource.m_SourceFile.str());
	}

	void Resource::_ReloadResource::operator() (const Ref<Shader>& texture)
	{
		APEX_LOG_INFO("Reloading shader from '{}'", resource.m_SourceFile.str());
	}

	// Unload
	void Resource::_UnloadResource::operator() (const Ref<VFS::IFile>& texture)
	{
		APEX_LOG_INFO("Unloading file '{}'", resource.m_SourceFile.str());
	}

	void Resource::_UnloadResource::operator() (const Ref<Texture>& texture)
	{
		APEX_LOG_INFO("Unloading texture from '{}'", resource.m_SourceFile.str());
		resource.m_Ptr = Ref<Texture>(nullptr);
	}

	void Resource::_UnloadResource::operator() (const Ref<Shader>& texture)
	{
		APEX_LOG_INFO("Unloading shader from '{}'", resource.m_SourceFile.str());
	}

}
