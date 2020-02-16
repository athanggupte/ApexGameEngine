#pragma once

namespace Apex {

	class RenderThread
	{
	public:
		virtual ~RenderThread();
		static void Init();

		static void ExecuteQueue();

	private:
		RenderThread();

	private:
		static RenderThread* s_Instance;
	};

}