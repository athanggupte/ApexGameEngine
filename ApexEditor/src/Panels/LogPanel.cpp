#include <apexed_pch.h>
#include "LogPanel.h"

#include "Apex/Core/Log.h"

namespace Apex {
	
	static ImVec4 GetLogLevelColor(uint8_t level)
	{
		switch (level) {
			case 0: return { 0.72f, 0.72f, 0.72f, 0.72f };    // Trace
			case 1: return { 1.f, 1.f, 1.f, 1.f };       // Debug
			case 2: return { 0.23f, 0.96f, 0.45f, 1.f }; // Info
			case 3: return { 0.74f, 0.58f, 0.26f, 1.f }; // Warn
			case 4: return { 0.91f, 0.12f, 0.12f, 1.f }; // Error
			case 5: return { 1.f, 0.f, 0.f, 1.f };       // Critical
		}
		return { 1.f, 1.f, 1.f, 1.f };
	}
	
	void LogPanel::AddLog(const std::string& log_msg, const LogMsgOptions& options)
	{
		auto oldSize = m_Buffer.size();
		m_Buffer.insert(m_Buffer.end(), log_msg.begin(), log_msg.end());
		if (oldSize == 0) {
			m_LineOffsets.emplace_back(0);
			m_LineOptions.emplace_back(options);
		}
		auto newSize = m_Buffer.size();
		auto it = oldSize + ((oldSize == 0) ? 0 : -1);
		for (; it < newSize-1; it++)
			if (m_Buffer[it] == '\n') {
				m_LineOffsets.emplace_back(it+1);
				m_LineOptions.emplace_back(options);
			}
	}
	
	void LogPanel::OnImGuiRender(bool* p_open)
	{
		if (!ImGui::Begin("Log", p_open)) {
			ImGui::End();
			return;
		}
		
		ImGui::SameLine();
		m_Filter.Draw("Filter", 100.f);
		ImGui::SameLine();
		bool clear = ImGui::Button("Clear");
		
		ImGui::Separator();
		ImGui::BeginChild("logspace", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		
		if (clear)
			Clear();
		
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		bool empty = m_Buffer.size() == 0;
		const char* buf_beg = (!empty) ? &m_Buffer.front() : EMPTY_STRING;
		const char* buf_end = (!empty) ? &m_Buffer.back() : EMPTY_STRING;
		if (m_Filter.IsActive()){
			for (auto line_no = 0; line_no < m_LineOffsets.size(); line_no++) {
				const char* line_start = buf_beg + m_LineOffsets[line_no];
				const char* line_end = (line_no + 1 < m_LineOffsets.size()) ? (buf_beg + m_LineOffsets[line_no + 1] - 1) : buf_end;
				if (m_Filter.PassFilter(line_start, line_end)) {
					ImGui::PushStyleColor(ImGuiCol_Text, GetLogLevelColor(m_LineOptions[line_no].LogLevel));
					ImGui::TextUnformatted(line_start, line_end);
					ImGui::PopStyleColor();
				}
			}
		} else {
			ImGuiListClipper clipper;
			clipper.Begin(m_LineOffsets.size());
			while (clipper.Step()) {
				for (auto line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++) {
					const char* line_start = buf_beg + m_LineOffsets[line_no];
					const char* line_end = (line_no + 1 < m_LineOffsets.size()) ? (buf_beg + m_LineOffsets[line_no + 1] - 1) : buf_end;
					ImGui::PushStyleColor(ImGuiCol_Text, GetLogLevelColor(m_LineOptions[line_no].LogLevel));
					ImGui::TextUnformatted(line_start, line_end);
					ImGui::PopStyleColor();
				}
			}
			clipper.End();
		}
		ImGui::PopStyleVar();
		
		if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.f);
		
		ImGui::EndChild();
		ImGui::End();
	}
	
	
} 
