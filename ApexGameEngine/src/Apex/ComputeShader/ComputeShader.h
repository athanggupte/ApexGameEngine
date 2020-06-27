#pragma once

namespace Apex {

	class ComputeShader
	{
	public:
		virtual ~ComputeShader() = default;

		virtual void Dispatch() = 0;
	};

}