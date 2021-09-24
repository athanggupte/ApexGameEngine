#pragma once

#include <vector>
#include <cstring>
#include <mutex>

#include <spdlog/sinks/base_sink.h>
#include <spdlog/details/null_mutex.h>
#include <imgui.h>

namespace Apex {
	
	struct LogMsgOptions
	{
		using LogLevel_t = uint8_t;
		
		std::string LoggerName;
		LogLevel_t LogLevel;
	};
	
	class LogPanel
	{
	public:
		LogPanel() : AutoScroll(true)
		{
			Clear();
		}
		
		void Clear()
		{
			m_Buffer.clear();
			m_LineOffsets.clear();
		}
		
		void AddLog(const std::string& log_msg, const LogMsgOptions& options);
		void OnImGuiRender(bool* p_open = nullptr);
		
	public:
		bool AutoScroll;
		
	private:
		constexpr static const char EMPTY_STRING[1] = "";
		std::vector<char> m_Buffer;
		ImGuiTextFilter m_Filter;
		std::vector<uint32_t> m_LineOffsets;
		std::vector<LogMsgOptions> m_LineOptions;
	};
	
	using LogPatternFlags_t = uint16_t;
	
	struct LogPattern {
		enum : LogPatternFlags_t
		{
			None            = 0,
			LogLevel        = 0b1,
			SourceLocation  = 0b1 << 1,
			SourceFunction  = 0b1 << 2,
			ThreadId        = 0b1 << 3,
			ProcessId       = 0b1 << 4,
			All             = 0b11111
		};
	};
	
	template<typename Mutex>
	class EditorLogSink : public spdlog::sinks::base_sink<Mutex>
	{
	public:
		EditorLogSink(LogPanel* panel) : m_LogPanel(panel)
		{
			assert(panel);
			SetPatternFlags(LogPattern::None);
		}
	
		void SetPattern(const std::string& pattern) { this->set_pattern(pattern); }
		
		void SetPatternFlags(LogPatternFlags_t flags)
		{
			std::string pattern;
			pattern = std::string("[%H:%M:%S.%e] <%n>")
					+ ((flags & LogPattern::LogLevel) ? " /%l" : "")
					+ ((flags & LogPattern::SourceLocation) ? " [%s:%#]" : "")
					+ ((flags & LogPattern::SourceFunction) ? " in `%!`" : "")
					+ ((flags & LogPattern::ThreadId) ? " [TID:%t]" : "")
					+ ((flags & LogPattern::ProcessId) ? " [PID:%P]" : "")
					+ " :: %v";
			this->set_pattern(pattern);
		}
		
	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override
		{
			if (!m_LogPanel) return;
			
			spdlog::memory_buf_t formattedMsg;
			spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formattedMsg);
			m_LogPanel->AddLog(fmt::to_string(formattedMsg), { std::string(msg.logger_name.data()), (uint8_t)msg.level });
			// std::cout << "LOG-MSG [" << msg.logger_name.data() << ", " << msg.level << "] : " << fmt::to_string(formattedMsg);
		}
		
		void flush_() override
		{
			// Flush
		}
		
	private:
		LogPanel* m_LogPanel;
		LogPatternFlags_t m_PatternFlags;
	};
	
	using EditorLogSink_mt = EditorLogSink<std::mutex>;
	using EditorLogSink_st = EditorLogSink<spdlog::details::null_mutex>;
	
}
