#include "apex_pch.h"
#include "Shader.h"

#include "Apex/Graphics/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Apex {

	Ref<Shader> Shader::Create(const fs::path& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLShader>(filepath);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
	}
	
	Ref<Shader> Shader::Create(const std::string& name, const std::string & vertexSrc, const std::string & fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
	}



	bool ShaderUniformTypeIsSampler(ShaderUniformType type)
	{
		return (static_cast<int32_t>(type) >= static_cast<int32_t>(ShaderUniformType::SAMPLER_1D)) && 
			(static_cast<int32_t>(type) < static_cast<int32_t>(ShaderUniformType::_COUNT));
	}

	const char* ShaderUniformTypeToString(ShaderUniformType type)
	{
		switch (type) {
		case ShaderUniformType::FLOAT: return "FLOAT";
		case ShaderUniformType::FLOAT_VEC2: return "FLOAT_VEC2";
		case ShaderUniformType::FLOAT_VEC3: return "FLOAT_VEC3";
		case ShaderUniformType::FLOAT_VEC4: return "FLOAT_VEC4";
		case ShaderUniformType::DOUBLE: return "DOUBLE";
		case ShaderUniformType::DOUBLE_VEC2: return "DOUBLE_VEC2";
		case ShaderUniformType::DOUBLE_VEC3: return "DOUBLE_VEC3";
		case ShaderUniformType::DOUBLE_VEC4: return "DOUBLE_VEC4";
		case ShaderUniformType::INT: return "INT";
		case ShaderUniformType::INT_VEC2: return "INT_VEC2";
		case ShaderUniformType::INT_VEC3: return "INT_VEC3";
		case ShaderUniformType::INT_VEC4: return "INT_VEC4";
		case ShaderUniformType::UNSIGNED_INT: return "UNSIGNED_INT";
		case ShaderUniformType::UNSIGNED_INT_VEC2: return "UNSIGNED_INT_VEC2";
		case ShaderUniformType::UNSIGNED_INT_VEC3: return "UNSIGNED_INT_VEC3";
		case ShaderUniformType::UNSIGNED_INT_VEC4: return "UNSIGNED_INT_VEC4";
		case ShaderUniformType::BOOL: return "BOOL";
		case ShaderUniformType::BOOL_VEC2: return "BOOL_VEC2";
		case ShaderUniformType::BOOL_VEC3: return "BOOL_VEC3";
		case ShaderUniformType::BOOL_VEC4: return "BOOL_VEC4";
		case ShaderUniformType::FLOAT_MAT2: return "FLOAT_MAT2";
		case ShaderUniformType::FLOAT_MAT3: return "FLOAT_MAT3";
		case ShaderUniformType::FLOAT_MAT4: return "FLOAT_MAT4";
		case ShaderUniformType::FLOAT_MAT2x3: return "FLOAT_MAT2x3";
		case ShaderUniformType::FLOAT_MAT2x4: return "FLOAT_MAT2x4";
		case ShaderUniformType::FLOAT_MAT3x2: return "FLOAT_MAT3x2";
		case ShaderUniformType::FLOAT_MAT3x4: return "FLOAT_MAT3x4";
		case ShaderUniformType::FLOAT_MAT4x2: return "FLOAT_MAT4x2";
		case ShaderUniformType::FLOAT_MAT4x3: return "FLOAT_MAT4x3";
		case ShaderUniformType::SAMPLER_1D: return "SAMPLER_1D";
		case ShaderUniformType::SAMPLER_2D: return "SAMPLER_2D";
		case ShaderUniformType::SAMPLER_3D: return "SAMPLER_3D";
		case ShaderUniformType::SAMPLER_CUBE: return "SAMPLER_CUBE";
		case ShaderUniformType::SAMPLER_1D_SHADOW: return "SAMPLER_1D_SHADOW";
		case ShaderUniformType::SAMPLER_2D_SHADOW: return "SAMPLER_2D_SHADOW";
		case ShaderUniformType::SAMPLER_1D_ARRAY: return "SAMPLER_1D_ARRAY";
		case ShaderUniformType::SAMPLER_2D_ARRAY: return "SAMPLER_2D_ARRAY";
		case ShaderUniformType::SAMPLER_1D_ARRAY_SHADOW: return "SAMPLER_1D_ARRAY_SHADOW";
		case ShaderUniformType::SAMPLER_2D_ARRAY_SHADOW: return "SAMPLER_2D_ARRAY_SHADOW";
		case ShaderUniformType::SAMPLER_2D_MULTISAMPLE: return "SAMPLER_2D_MULTISAMPLE";
		case ShaderUniformType::SAMPLER_2D_MULTISAMPLE_ARRAY: return "SAMPLER_2D_MULTISAMPLE_ARRAY";
		case ShaderUniformType::SAMPLER_CUBE_SHADOW: return "SAMPLER_CUBE_SHADOW";
		case ShaderUniformType::SAMPLER_BUFFER: return "SAMPLER_BUFFER";
		case ShaderUniformType::SAMPLER_2D_RECT: return "SAMPLER_2D_RECT";
		case ShaderUniformType::SAMPLER_2D_RECT_SHADOW: return "SAMPLER_2D_RECT_SHADOW";
		default: APEX_CORE_CRITICAL("Unknown Shader uniform type!");
		}
	}

}
