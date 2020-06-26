#include "Terminal.h"
//#include <cstdint>

Terminal::Terminal(/* COORD size */)
	: m_ConsoleHandle{ INVALID_HANDLE_VALUE }, m_InputHandle(INVALID_HANDLE_VALUE), m_OutputHandle(INVALID_HANDLE_VALUE), m_Process{}
{
}

void Terminal::Init()
{
	CreateConsole({});
	m_Process.Init(m_ConsoleHandle);
}

void Terminal::DeInit()
{
	ClosePseudoConsole(m_ConsoleHandle);
	if (m_InputHandle != INVALID_HANDLE_VALUE) CloseHandle(m_InputHandle);
	if (m_OutputHandle != INVALID_HANDLE_VALUE) CloseHandle(m_OutputHandle);
}

unsigned long Terminal::ReadFromPTY(char* buffer)
{
	bool bRead{ false };
	DWORD bufferDataSize;

	bRead = ReadFile(m_OutputHandle, buffer, MAX_BUF_LEN, &bufferDataSize, NULL);

	return bufferDataSize;
}

bool Terminal::WriteToPTY(char* buffer)
{
	DWORD size = strlen(buffer);
	return WriteFile(m_InputHandle, buffer, size, &size, NULL);
}

HRESULT Terminal::CreateConsole(COORD size)
{
	(void)size;

	HRESULT hr{ E_UNEXPECTED };
	HANDLE hPTYPipeIn{ INVALID_HANDLE_VALUE };		// Input for child process
	HANDLE hPTYPipeOut{ INVALID_HANDLE_VALUE };		// Output for child process


	if (CreatePipe(&hPTYPipeIn, &m_InputHandle, NULL, 0) && CreatePipe(&m_OutputHandle, &hPTYPipeOut, NULL, 0)) {
	#ifndef GET_SIZE_FROM_USER
		COORD consoleSize{};
		CONSOLE_SCREEN_BUFFER_INFO csbi{};
		HANDLE hConsole{ GetStdHandle(STD_OUTPUT_HANDLE) };
		if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
			consoleSize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
			consoleSize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		}
		size = consoleSize;
	#endif
		hr = CreatePseudoConsole(size, hPTYPipeIn, hPTYPipeOut, NULL, &m_ConsoleHandle);

		if (hPTYPipeIn != INVALID_HANDLE_VALUE) CloseHandle(hPTYPipeIn);
		if (hPTYPipeOut != INVALID_HANDLE_VALUE) CloseHandle(hPTYPipeOut);
	}

	return hr;
}

Process::~Process()
{
	if (m_StartupInfo.lpAttributeList)
		DeInit();
}

HRESULT Process::Init(HPCON hPCon)
{
	HRESULT hr{ E_UNEXPECTED };

	/* Initialize Startup info with ConPTY handle */
	{
		m_StartupInfo.StartupInfo.cb = sizeof(STARTUPINFOEX);

		/* Find size of attribute list */
		size_t attrListSize;
		InitializeProcThreadAttributeList(NULL, 1, NULL, &attrListSize);

		/* Allocate thread attribute list of correct size */
		m_StartupInfo.lpAttributeList = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(HeapAlloc(GetProcessHeap(), NULL, attrListSize));

		/* Initialize thread attribute list */
		if (m_StartupInfo.lpAttributeList && InitializeProcThreadAttributeList(m_StartupInfo.lpAttributeList, 1, NULL, &attrListSize)) {

			/* Set Pseudo Console attribute */
			hr = UpdateProcThreadAttribute(
				m_StartupInfo.lpAttributeList,
				NULL,
				PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
				hPCon,
				sizeof(HPCON),
				NULL,
				NULL)
				? S_OK : HRESULT_FROM_WIN32(GetLastError());
		}
		else {
			HeapFree(GetProcessHeap(), NULL, m_StartupInfo.lpAttributeList);
			hr = HRESULT_FROM_WIN32(GetLastError());
			return hr;
		}
	}

	wchar_t cmdline[]{ L"cmd" };

	hr = CreateProcess(
		NULL,
		cmdline,
		NULL,
		NULL,
		FALSE,
		EXTENDED_STARTUPINFO_PRESENT,
		NULL,
		NULL,
		&m_StartupInfo.StartupInfo,
		&m_ProcInfo)
		? S_OK : HRESULT_FROM_WIN32(GetLastError());

	//if (hr == S_OK)
		//WaitForSingleObject(m_ProcInfo.hProcess, 5000);

	return hr;
}

void Process::DeInit()
{
	CloseHandle(m_ProcInfo.hThread);
	CloseHandle(m_ProcInfo.hProcess);
	DeleteProcThreadAttributeList(m_StartupInfo.lpAttributeList);
	HeapFree(GetProcessHeap(), NULL, m_StartupInfo.lpAttributeList);
}


bool TerminalBuffer::s_DebugMode = false;

