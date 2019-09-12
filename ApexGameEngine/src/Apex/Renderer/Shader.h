#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Apex {

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const std::string& GetName() const = 0;

		virtual const std::unordered_map<std::string, uint32_t>& GetActiveUniformLocations() = 0;

		virtual void SetUniInt(const std::string& name, int value) = 0;
		virtual void SetUniFloat1(const std::string& name, float value) = 0;
		virtual void SetUniFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetUniFloat3(const std::string& name, const glm::vec3&  value) = 0;
		virtual void SetUniFloat4(const std::string& name, const glm::vec4&  value) = 0;
		virtual void SetUniMat4(const std::string& name, const glm::mat4& matrix) = 0;
		
		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};

	class ShaderLibrary
	{
	public:
		void Add(const std::string& name, const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);

		size_t GetNumAvailableShaders();
		std::vector<std::string> ListAllShaders();
		Ref<Shader> GetShader(const std::string& name);
 		bool Exists(const std::string & name);
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};

}