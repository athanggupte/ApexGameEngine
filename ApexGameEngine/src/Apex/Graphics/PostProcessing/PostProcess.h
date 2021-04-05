#pragma once

namespace Apex {
	
	class Shader;
	
	class PostProcess
	{
	public:
		static void Init();
		static void Shutdown();
		
		static void Submit(const Ref<Shader>&);
		
	private:
	};
	
}
