#pragma once

#include "RenderCommands.h"

#include <queue>
#include <mutex>
//#include <functional>

namespace Apex {
	
	struct RenderQueueItem {
		RenderCommandType type;

		RenderQueueItem(RenderCommandType type)
		{
			this->type = type;
		}

	private:
		RenderQueueItem() {}
	};

	class RenderQueue
	{
	public:
		virtual ~RenderQueue();

		static void Init();
		
		static void Push(RenderCommandType type) { s_Instance->m_Queue->push(std::make_shared<RenderQueueItem>(type)); }
		static void Push(const Ref<RenderQueueItem>& item) { s_Instance->m_Queue->push(item); }
		static void Pop() { s_Instance->m_Queue->pop(); }

		static const Ref<RenderQueueItem>& Front() { return s_Instance->m_Queue->front(); }
		static const Ref<RenderQueueItem>& Back() { return s_Instance->m_Queue->back(); }
		static bool IsEmpty() { return s_Instance->m_Queue->empty(); }

		//static void AttainLock(std::function<bool(void)>&& fun);
		static std::unique_lock<std::mutex>& Lock();
		static void Notify() { s_Instance->condition_variable.notify_one(); }
		
		inline static void Wait(std::unique_lock<std::mutex>& lock, std::function<bool(void)>&& fun)
		{
			s_Instance->condition_variable.wait(lock, std::forward<std::function<bool(void)>>(fun));
		}

	private:
		RenderQueue();

		static void Execute();

	private:
		static RenderQueue* s_Instance;
		std::queue<Ref<RenderQueueItem>>* m_Queue;
		std::thread* m_Thread;
		std::mutex mutex;
		std::condition_variable condition_variable;
	};

}