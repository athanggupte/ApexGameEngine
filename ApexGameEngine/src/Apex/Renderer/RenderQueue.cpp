#include "apex_pch.h"
#include "RenderQueue.h"

namespace Apex {

	RenderQueue* RenderQueue::s_Instance = nullptr;

	RenderQueue::RenderQueue()
	{
		m_Queue = new std::queue<Ref<RenderQueueItem>>(); 
	}

	Apex::RenderQueue::~RenderQueue()
	{
	}

	void RenderQueue::Init()
	{
		s_Instance = new RenderQueue(); 
	}

	void RenderQueue::AttainLock(std::function<bool(void)>&& fun)
	{
		Wait(Lock(), std::forward<std::function<bool(void)>>(fun));
	}

	inline std::unique_lock<std::mutex>& RenderQueue::Lock()
	{
		std::unique_lock<std::mutex> lock(s_Instance->mutex);
		return lock;
	}
	
	inline void RenderQueue::Wait(std::unique_lock<std::mutex>& lock, std::function<bool(void)>&& fun)
	{
		s_Instance->condition_variable.wait(lock, std::forward<std::function<bool(void)>>(fun));
	}

}