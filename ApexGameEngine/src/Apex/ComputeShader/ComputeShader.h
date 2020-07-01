#pragma once

namespace Apex {

	class ComputeShader
	{
	public:

		virtual ~ComputeShader() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Dispatch(uint32_t nWorkGrpsX, uint32_t nWorkGrpsY, uint32_t nWorkGrpsZ) = 0;

		static Ref<ComputeShader> Create(const std::string& filepath);
		static Ref<ComputeShader> Create(const std::string& name, const std::string& source);
	};

}