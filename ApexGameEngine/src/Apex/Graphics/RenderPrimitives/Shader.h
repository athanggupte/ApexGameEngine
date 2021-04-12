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

		virtual const std::unordered_map<std::string, uint32_t>& GetActiveUniformLocations() const = 0;
		virtual const std::vector<std::tuple<std::string, uint32_t, size_t>> GetActiveUniformData() const = 0;

		// Int types
		virtual void SetUniInt1(const std::string& name, int value) const = 0;
		virtual void SetUniInt2(const std::string& name, const glm::ivec2& value) const = 0;
		virtual void SetUniInt3(const std::string& name, const glm::ivec3& value) const = 0;
		virtual void SetUniInt4(const std::string& name, const glm::ivec4& value) const = 0;
		
		// Int array types
		virtual void SetUniInt1v(const std::string& name, int values[], size_t count) const = 0;
		virtual void SetUniInt2v(const std::string& name, glm::ivec2 values[], size_t count) const = 0;
		virtual void SetUniInt3v(const std::string& name, glm::ivec3 values[], size_t count) const = 0;
		virtual void SetUniInt4v(const std::string& name, glm::ivec4 values[], size_t count) const = 0;
		
		// Float types
		virtual void SetUniFloat1(const std::string& name, float value) const = 0;
		virtual void SetUniFloat2(const std::string& name, const glm::vec2& value) const = 0;
		virtual void SetUniFloat3(const std::string& name, const glm::vec3& value) const = 0;
		virtual void SetUniFloat4(const std::string& name, const glm::vec4& value) const = 0;
		
		// Float array types
		virtual void SetUniFloat1v(const std::string& name, float values[], size_t count) const = 0;
		virtual void SetUniFloat2v(const std::string& name, glm::vec2 values[], size_t count) const = 0;
		virtual void SetUniFloat3v(const std::string& name, glm::vec3 values[], size_t count) const = 0;
		virtual void SetUniFloat4v(const std::string& name, glm::vec4 values[], size_t count) const = 0;
		
		// Matrix types
		virtual void SetUniMat4(const std::string& name, const glm::mat4& matrix) const = 0;
		virtual void SetUniMat4v(const std::string& name, glm::mat4 matrices[], size_t count) const = 0;
		
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
