#pragma once

#include <functional>

namespace Apex {

	class ScopeGuard
	{
	public:
		~ScopeGuard()
		{
			m_CleanupFunc();
		}

		template<typename Func>
		ScopeGuard(const Func& cleanupFunc)
			: m_CleanupFunc(cleanupFunc)
		{
		}

		template<typename Func>
		explicit ScopeGuard(Func&& cleanupFunc)
			: m_CleanupFunc(cleanupFunc)
		{
		}

		ScopeGuard(ScopeGuard&& other)
			: m_CleanupFunc(std::move(other.m_CleanupFunc))
		{
			other.dismiss();
		}

		void dismiss()
		{
			m_CleanupFunc = [] {};
		}

	private:
		std::function<void(void)> m_CleanupFunc;
	};

}

#define APEX_SCOPE_GUARD const Apex::ScopeGuard ANONYMOUS_VAR(scope_guard_) = [&]()
