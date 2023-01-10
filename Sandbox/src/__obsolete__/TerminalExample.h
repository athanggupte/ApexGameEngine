#pragma once
/* Layer for making a terminal emulator using ImGui */

#include "Terminal.h"
#include "ChildProcess.h"

static std::mutex s_TerminalBufferMutex;

#define NUMBER_OF_TERMINAL_FRAMES 10
#define TERMINAL_DISPLAY_BUFFER_SIZE NUMBER_OF_TERMINAL_FRAMES * Terminal::MAX_BUF_LEN

class TerminalLayer : public Apex::Layer
{
public:
	TerminalLayer()
		: m_TerminalBuffer(TERMINAL_DISPLAY_BUFFER_SIZE), m_DeviceName{}, m_CommandBuffer{},
		  m_LogcatProcess(), m_ADBShellProcess(), m_Terminal()
	{
		APEX_LOG_DEBUG("Capacity of Circular Buffer : {0}", m_TerminalBuffer.GetCapacity());
		//m_TerminalBuffer.reserve(TERMINAL_DISPLAY_BUFFER_SIZE);
	}

	virtual void OnAttach() override
	{
		m_ADBShellProcess.Init("adb shell");
		//m_Terminal.Init();

		std::thread([&]() {
			{
				char buf[32]{};
				size_t size;
				m_ADBShellProcess.WriteToPipe("getprop ro.product.device\r\n");
				size = m_ADBShellProcess.ReadFromPipe(buf);
				buf[size - 1] = '\0';
				buf[size - 2] = '\0';
				strcat(m_DeviceName, buf);

				strcat(m_DeviceName, " [");

				m_ADBShellProcess.WriteToPipe("getprop ro.product.model\r\n");
				size = m_ADBShellProcess.ReadFromPipe(buf);
				buf[size - 1] = '\0';
				buf[size - 2] = '\0';
				strcat(m_DeviceName, buf);

				strcat(m_DeviceName, "] > ");
			}
			uint32_t bytesRead = 0;
			//while (bytesRead >= 0) {
			while (m_ADBShellProcess.IsStarted() && bytesRead >= 0) {
				char buffer[Terminal::MAX_BUF_LEN]{};
				//bytesRead = m_Terminal.ReadFromPTY(buffer);
				bytesRead = m_ADBShellProcess.ReadFromPipe(buffer);
				{
					//std::lock_guard<std::mutex> lock(s_TerminalBufferMutex);
					m_TerminalBuffer.PushBack("");
					m_TerminalBuffer.PushBack(buffer);
				}
				//std::this_thread::sleep_for(std::chrono::milliseconds(200));
			}
			APEX_LOG_DEBUG("Stopping Terminal Read Thread");
		}).detach();
	}

	virtual void OnDetach() override
	{
		m_ADBShellProcess.DeInit();
		m_LogcatProcess.DeInit();
	}

	virtual void OnUpdate() override
	{
		auto secs = Apex::Timer::GetSeconds();

		Apex::RenderCommands::SetClearColor({ sin(0.85 * secs), sin(0.15f * secs), sin(0.55f * secs), 1.0f });
		Apex::RenderCommands::Clear();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::ShowDemoWindow();

		/*ImGui::Begin("Log");
		if (m_LogcatProcess.IsStarted()) {
			//ImGui::InputText()
			if (ImGui::Button("Stop"))
				m_LogcatProcess.DeInit();
			ImGui::BeginChild("Logcat");
			{
				APEX_PROFILE_SCOPE("Print PTY Buffer");
				ImGui::TextUnformatted(m_TerminalBuffer.GetBufferOne());
				ImGui::TextUnformatted(m_TerminalBuffer.GetBufferTwo());
			}
			ImGui::EndChild();
		}
		else {
			if (ImGui::Button("Start"))
				StartLogcat();
		}
		ImGui::End();*/

		//char windowName[40]{ "Shell: " };
		//strcpy(windowName + strlen(windowName), m_DeviceName);
		//ImGui::Begin(windowName);
		ImGui::Begin("Shell");
		//ImGui::BeginChild("shell");
		ImGui::TextUnformatted(m_DeviceName);
		ImGui::TextUnformatted(m_TerminalBuffer.GetBufferOne());
		ImGui::TextUnformatted(m_TerminalBuffer.GetBufferTwo());
		//ImGui::EndChild();
		//ImGui::InputText("Command:", m_CommandBuffer, 64); ImGui::SameLine();
		//ImGui::Button("Submit");
		ImGui::End();

		ImGui::Begin("Commands");
		ImGui::InputText("Command:", m_CommandBuffer, 61); ImGui::SameLine();
		if (ImGui::Button("Submit")) {
			//auto len = strlen(m_CommandBuffer);
			//m_CommandBuffer[len] = '\r';
			//m_CommandBuffer[len + 1] = '\n';
			//m_CommandBuffer[len + 1] = '\0';
			m_ADBShellProcess.SendCommand(m_CommandBuffer);
			memset(m_CommandBuffer, 0, 64);
			//m_Terminal.WriteToPTY(m_CommandBuffer);
		}
		if (ImGui::Button("Stop")) {
			//SetConsoleCtrlHandler(NULL, TRUE);
			m_ADBShellProcess.SendCtrlC();
		}
		ImGui::End();
	}

	void StartLogcat()
	{
		m_LogcatProcess.Init("adb logcat");

		std::thread([&]() {
			uint32_t bytesRead = 0;
			while (m_LogcatProcess.IsStarted() && bytesRead >= 0) {
				// APEX_PROFILE_SCOPE("Read PTY Buffer");
				char buffer[Terminal::MAX_BUF_LEN]{};
				//bytesRead = m_Terminal.ReadFromPTY(buffer);
				bytesRead = m_LogcatProcess.ReadFromPipe(buffer);
				{
					//std::lock_guard<std::mutex> lock(s_TerminalBufferMutex);
					m_TerminalBuffer.PushBack(buffer);
				}
				//std::this_thread::sleep_for(std::chrono::milliseconds(200));
			}
			}).detach();

	}

private:
	Terminal m_Terminal;
	ChildProcess m_LogcatProcess, m_ADBShellProcess;
	char m_DeviceName[32];
	char m_CommandBuffer[64];
	TerminalBuffer m_TerminalBuffer;
}; 
