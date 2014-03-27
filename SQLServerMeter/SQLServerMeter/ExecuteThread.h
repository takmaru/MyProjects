#pragma once

#include <Windows.h>
#include "Setting.h"

class CExecuteThread {
public:
	explicit CExecuteThread(const CSetting& setting);
	~CExecuteThread();

public:
	bool start();
	bool stop(DWORD waitTimeout = 0);
	bool isRunning(DWORD waitTimeout);

private:
	static unsigned __stdcall ThreadHandler(void* param);
	unsigned execute();

private:
	HANDLE m_threadHandle;
	HANDLE m_stopEvent;

	CSetting m_setting;
};
