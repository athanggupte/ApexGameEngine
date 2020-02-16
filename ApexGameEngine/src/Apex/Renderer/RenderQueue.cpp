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
		s_Instance->m_Thread = new std::thread(Execute);
		APEX_CORE_DEBUG("Created RenderThread : Thread id : {0}", s_Instance->m_Thread->get_id());
	}

	/*void RenderQueue::AttainLock(std::function<bool(void)>&& fun)
	{
		Wait(Lock(), std::forward<std::function<bool(void)>>(fun));
	}*/

	std::unique_lock<std::mutex>& RenderQueue::Lock()
	{
		std::unique_lock<std::mutex> lock(s_Instance->mutex);
		return lock;
	}

	void RenderQueue::Execute()
	{	
		while (true) {
			{
				std::unique_lock<std::mutex> lock(s_Instance->mutex);
				s_Instance->condition_variable.wait(lock, []() {
					if (RenderQueue::IsEmpty())
						return false;
					else {
						APEX_CORE_DEBUG("\t| WAIT : RenderQueue::Back() = {0}", (int)RenderQueue::Back()->type);
						return RenderQueue::Back()->type == RenderCommandType::EndScene;
					}
				});
				APEX_CORE_TRACE("ExecuteQueue has attained the LOCK");
				Ref<RenderQueueItem> front = RenderQueue::Front();
				int i = 0;
				while (front->type != RenderCommandType::EndScene) {
					APEX_CORE_INFO("{0} | Command : {1}", i, (int)front->type);
					RenderQueue::Pop();
					front = RenderQueue::Front();
				}
			}
			APEX_CORE_TRACE("ExecuteQueue has finished execution");
		}
	}

}