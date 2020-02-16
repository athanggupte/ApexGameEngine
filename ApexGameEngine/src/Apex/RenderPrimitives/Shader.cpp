#include "apex_pch.h"
#include "Shader.h"

#include "Apex/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Apex {

	Ref<Shader> Shader::Create(const std::string & filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLShader>(filepath);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
	}
	
	Ref<Shader> Shader::Create(const std::string& name, const std::string & vertexSrc, const std::string & fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
	}
	

	/////////////////////////////////////////////////////////////////////////
	//                            ShaderLibrary                            //
	/////////////////////////////////////////////////////////////////////////

	void ShaderLibrary::Add(const std::string & name, const Ref<Shader>& shader)
	{
		APEX_CORE_ASSERT(!Exists(name), "Shader already exists");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::string & filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}
	
	Ref<Shader> ShaderLibrary::Load(const std::string & name, const std::string & filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}
	
	size_t ShaderLibrary::GetNumAvailableShaders()
	{
		return m_Shaders.size();
	}

	std::vector<std::string> ShaderLibrary::ListAllShaders()
	{
		std::vector<std::string> shaderList(m_Shaders.size());
		auto key_selector = [](auto pair) { return pair.first; };
		std::transform(m_Shaders.begin(), m_Shaders.end(), shaderList.begin(), key_selector);
		return shaderList;
	}

	Ref<Shader> ShaderLibrary::GetShader(const std::string & name)
	{
		APEX_CORE_ASSERT(Exists(name), "Shader not found");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name)
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}