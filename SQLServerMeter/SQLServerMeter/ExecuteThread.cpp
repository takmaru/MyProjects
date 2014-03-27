#include "stdafx.h"
#include "ExecuteThread.h"

#include <process.h>

#include <MyLib/tstring/tstring.h>

#include <ODBCLibInclude.h>

CExecuteThread::CExecuteThread(const CSetting& setting):
	m_threadHandle(NULL), m_stopEvent(NULL), m_setting(setting) {
}
CExecuteThread::~CExecuteThread() {
}

bool CExecuteThread::start() {
	m_stopEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if(m_stopEvent == NULL) {
		return false;
	}
	m_threadHandle =
		reinterpret_cast<HANDLE>(
			::_beginthreadex(NULL, 0, CExecuteThread::ThreadHandler, this, 0, NULL)
		);
	if(m_threadHandle == NULL) {
		::CloseHandle(m_stopEvent);
		return false;
	}
	return true;
}
bool CExecuteThread::stop(DWORD waitTimeout/*= 0*/) {
	::SetEvent(m_stopEvent);
	return this->isRunning(waitTimeout);
}
bool CExecuteThread::isRunning(DWORD waitTimeout) {
	return (::WaitForSingleObject(m_threadHandle, waitTimeout) != WAIT_OBJECT_0);
}

unsigned __stdcall CExecuteThread::ThreadHandler(void* param) {
	return reinterpret_cast<CExecuteThread*>(param)->execute();
}
unsigned CExecuteThread::execute() {

	for(int i = 0; i < m_setting.execCount(); i++) {
		if(::WaitForSingleObject(m_stopEvent, 0) == WAIT_OBJECT_0) {
			break;
		}
		
		ODBCLib::CODBCSession session;

		session.setServerName(std::tstring(m_setting.server() + std::tstring(_T("\\")) + m_setting.instance()).c_str());
		session.setDatabaseName(m_setting.database().c_str());
		if(session.startSession()) {
			std::shared_ptr<ODBCLib::CTransaction> tran = session.beginTransaction();
			tran->commit();

			session.endSession();
		}
	}

	return 0;
}
