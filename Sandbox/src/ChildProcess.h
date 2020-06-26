#pragma once

#include <Windows.h>
#include <cstdio>
#include <tchar.h>
#include <strsafe.h>

#define BIFSIZE 4096

class ChildProcess
{
public:
	ChildProcess()
		: m_OutputHandle{ INVALID_HANDLE_VALUE }, m_InputHandle{ INVALID_HANDLE_VALUE },
		m_StartupInfo{}, m_ProcInfo{}, m_Started{ false }
	{
	}

	void Init(const char* cmdline)
	{
		AllocConsole();
		CreateChildProcess(cmdline);
		m_Started = true;
	}

	void SendCtrlC()
	{
		AttachConsole(m_ProcInfo.dwProcessId);
		SetConsoleCtrlHandler(NULL, TRUE);
		GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
	}

	void SendCommand(char* buffer) {
		auto len = strlen(buffer);
		buffer[len] = '\r'; 
		buffer[len + 1] = '\n';
		WriteToPipe(buffer);
	}

	void WriteToPipe(const char* buffer)
	{
		DWORD size = strlen(buffer);
		DWORD sizeWritten;
		if (!WriteFile(m_InputHandle, buffer, size, &sizeWritten, NULL))
			APEX_CORE_ERROR("Cannot write into child process");
		else
			APEX_CORE_DEBUG("Wrote {} bytes", sizeWritten);
	}

	unsigned long ReadFromPipe(char* buffer)
	{
		bool bRead{ false };
		DWORD bufferDataSize;

		bRead = ReadFile(m_OutputHandle, buffer, 1024, &bufferDataSize, NULL);

		APEX_CORE_DEBUG("Read {} bytes", bufferDataSize);

		return bufferDataSize;
	}

	void DeInit()
	{
		CloseHandle(m_InputHandle);
		CloseHandle(m_OutputHandle);

		CloseHandle(m_ProcInfo.hProcess);
		CloseHandle(m_ProcInfo.hThread);
		FreeConsole();
		m_Started = false;
	}

	bool IsStarted() { return m_Started; }

private:
	void CreateChildProcess(const char* cmdline) {
		TCHAR  *cmdLine;
		
#ifdef UNICODE
		int wLen = MultiByteToWideChar(CP_ACP, 0, cmdline, -1, NULL, 0);
		cmdLine = new WCHAR[wLen];
		MultiByteToWideChar(CP_ACP, 0, cmdline, -1, (LPWSTR)cmdLine, wLen);
#endif // UNICODE


		BOOL success = FALSE;

		HANDLE child_in_rd = NULL;
		HANDLE child_in_wr = NULL;
		HANDLE child_out_rd = NULL;
		HANDLE child_out_wr = NULL;

		SECURITY_ATTRIBUTES saAttr;
		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		saAttr.bInheritHandle = TRUE;
		saAttr.lpSecurityDescriptor = NULL;

		CreatePipe(&child_out_rd, &child_out_wr, &saAttr, 0);
		SetHandleInformation(child_out_rd, HANDLE_FLAG_INHERIT, 0);

		CreatePipe(&child_in_rd, &child_in_wr, &saAttr, 0);
		SetHandleInformation(child_in_wr, HANDLE_FLAG_INHERIT, 0);

		m_InputHandle = child_in_wr;
		m_OutputHandle = child_out_rd;

		m_StartupInfo.cb = sizeof(STARTUPINFOEX);

		m_StartupInfo.hStdError = child_out_wr;
		m_StartupInfo.hStdOutput = child_out_wr;
		m_StartupInfo.hStdInput = child_in_rd;
		m_StartupInfo.dwFlags |= STARTF_USESTDHANDLES;

		success = CreateProcess(
			NULL,				// application name
			cmdLine,			// command line
			NULL,				// process security attr
			NULL,				// primary thread security attr
			TRUE,				// handles are inherited
			0,					// creation flags
			NULL,				// use parent's environment
			NULL,				// use parent's current directory
			&m_StartupInfo,		// STARTUPINFO pointer
			&m_ProcInfo);		// retrieves PROCESS_INFORMATION

		CloseHandle(child_in_rd);
		CloseHandle(child_out_wr);

		delete[] cmdLine;
	}

private:
	HANDLE m_InputHandle, m_OutputHandle;
	STARTUPINFO m_StartupInfo;
	PROCESS_INFORMATION m_ProcInfo;
	bool m_Started;
};