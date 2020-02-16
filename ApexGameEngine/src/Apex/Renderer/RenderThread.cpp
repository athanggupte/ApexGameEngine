#include "apex_pch.h"
#include "RenderThread.h"

//#include "RenderCommands.h"
#include "RenderQueue.h"

#include <thread>
#include <functional>

namespace Apex {

	RenderThread* RenderThread::s_Instance = nullptr;

	RenderThread::RenderThread()
	{
	}

	RenderThread::~RenderThread()
	{
	}

	void RenderThread::Init()
	{
		s_Instance = new RenderThread();
	}

	void RenderThread::ExecuteQueue()
	{
		RenderQueue::AttainLock([]() { return RenderQueue::Back()->type == RenderCommandType::EndScene; });
		Ref<RenderQueueItem> front = RenderQueue::Front();
		int i = 0;
		while (front->type != RenderCommandType::EndScene) {
			APEX_CORE_INFO("{0} | Command : {1}", i, (int)front->type);
			RenderQueue::Pop();
			front = RenderQueue::Front();
		}
	}

}