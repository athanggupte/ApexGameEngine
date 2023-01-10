#pragma once

//using uint32_t = unsigned long;

#ifdef APEX_PLATFORM_WINDOWS

#include <Windows.h>
#include <process.h>

class Process
{
public:
	Process()
		: m_StartupInfo{}, m_ProcInfo{}
	{
	}

	~Process();

	HRESULT Init(HPCON hPCon);
	void DeInit();

private:
	STARTUPINFOEX m_StartupInfo;
	PROCESS_INFORMATION m_ProcInfo;
};


class Terminal
{
public:
	Terminal(/* COORD size */);
	void Init();
	void DeInit();

	unsigned long ReadFromPTY(char* buffer);
	bool WriteToPTY(char* buffer);

private:
	HRESULT CreateConsole(COORD size);

public:
#define TERMINALMAXBUFLEN 1024
	static const unsigned long MAX_BUF_LEN = TERMINALMAXBUFLEN;

private:
	HPCON m_ConsoleHandle;
	Process m_Process;
	HANDLE m_OutputHandle, m_InputHandle;
};

#include <cstdio>
#include <string>

//static size_t min(size_t a, size_t b);

class TerminalBuffer
{
public:
	static bool s_DebugMode;

	static void DebugPrint(const TerminalBuffer& tbuf)
	{
		printf(__FUNCTION__ " Called\n");

		printf("\n****************DEBUG****************\n");
		for (size_t i = 0; i < tbuf.m_Capacity + 1; i++) {
			printf(" {%c : ", tbuf.m_BufferBase[i]);
			printf("%d} ", tbuf.m_BufferBase[i]);
		}
		printf("\n*************************************\n");
	}

#define DEBUG_TBUF(tbuf) if(TerminalBuffer::s_DebugMode) TerminalBuffer::DebugPrint(tbuf)

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

public:
	TerminalBuffer(size_t capacity)
		: m_Capacity(capacity)
	{
		m_BufferBase = new char[capacity + 2];
		m_BufferIndex = 0;
		memset((char*)m_BufferBase, 0, m_Capacity + 2);
		m_Size = 0;

		DEBUG_TBUF(*this);
	}

	void PushBack(const char* buf, size_t size = 0)
	{
		if (!size)
			size = strlen(buf);

		auto sizeToCopyToEnd = min(m_Capacity - m_BufferIndex + 1, size);
		memcpy((char*)m_BufferBase + m_BufferIndex, buf, sizeToCopyToEnd);
		m_BufferIndex += sizeToCopyToEnd;
		m_Size += sizeToCopyToEnd;

		size -= sizeToCopyToEnd;
		if (size)	// if there are characters in buffer remaining then overwrite from the start
		{
			m_BufferIndex = 0;
			memcpy((char*)m_BufferBase + m_BufferIndex, buf + sizeToCopyToEnd, size);
			m_BufferIndex += size;
			m_Size = m_Capacity;
		}
		*((char*)m_BufferBase + m_BufferIndex) = '\0';


		DEBUG_TBUF(*this);
	}

	void Clear()
	{
		m_BufferIndex = 0;
		memset((char*)m_BufferBase + m_BufferIndex, 0, m_Capacity);
		m_Size = 0;

		DEBUG_TBUF(*this);
	}

	const char* GetBufferOne()
	{
		return (m_BufferBase + m_BufferIndex + 1);
	}

	const char* GetBufferTwo()
	{
		return (m_BufferBase);
	}

	size_t GetCapacity()
	{
		return m_Capacity;
	}

private:
	const char* m_BufferBase;
	const size_t m_Capacity;
	//char* m_BufferTop;
	size_t m_BufferIndex;
	size_t m_Size;
};

#endif // APEX_PLATFORM_WINDOWS