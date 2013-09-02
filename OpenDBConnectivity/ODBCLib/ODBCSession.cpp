#include "stdafx.h"
#include "ODBCSession.h"

#include "EnvironmentHandle.h"
#include "ConnectionHandle.h"
#include "ODBCStatement.h"
#include "DiagInfo.h"

namespace {
	static const std::wstring defaultDriverName = L"{SQL Server Native Client 10.0}";
};

ODBCLib::CODBCSession::CODBCSession():
	m_isConnecting(false),
	m_environmentHandle(new CEnvironmentHandle()), m_ODBCVersion(SQL_OV_ODBC3),
	m_connectionHandle(),
	m_driverName(defaultDriverName), m_serverName(), m_databaseName(), m_isTrutedConnection(true) {
}

ODBCLib::CODBCSession::~CODBCSession() {
	endSession();
}

bool ODBCLib::CODBCSession::startSession() {
	if(isConnecting()) {
		return false;
	}

	bool result = false;
	SQLRETURN ret = m_environmentHandle->setVersion(m_ODBCVersion);
	if(ret == SQL_SUCCESS) {
		m_connectionHandle.reset(new CConnectionHandle(m_environmentHandle));
		if(m_connectionHandle->isHandleEnable()) {
			ret = m_connectionHandle->connect(connectionString().c_str());
			if(ret == SQL_SUCCESS) {
				result = true;
			} else if(ret == SQL_SUCCESS_WITH_INFO) {
				result = true;
				std::wcerr << ODBCLib::CDiagInfo(m_connectionHandle).description() << std::endl;
			} else {
				std::wcerr << L"CODBCSession::startSession() CConnectionHandle::connect()=" << ret << std::endl <<
					ODBCLib::CDiagInfo(m_connectionHandle).description() << std::endl;
			}
		}
	} else {
		std::wcerr << L"CODBCSession::startSession() CEnvironmentHandle::setVersion()=" << ret << std::endl <<
			ODBCLib::CDiagInfo(m_environmentHandle).description() << std::endl;
	}
	return result;
}
bool ODBCLib::CODBCSession::endSession() {
	if(!isConnecting()) {
		return false;
	}

	bool result = false;
	SQLRETURN ret = m_connectionHandle->disconnect();
	if(ret == SQL_SUCCESS) {
		m_connectionHandle.reset();
		result = true;
	} else {
		std::wcerr << L"CODBCSession::endSession() CConnectionHandle::disconnect()=" << ret << std::endl <<
			ODBCLib::CDiagInfo(m_connectionHandle).description() << std::endl;
	}
	return result;
}

std::shared_ptr<ODBCLib::CTransaction> ODBCLib::CODBCSession::beginTransaction() {
	return std::shared_ptr<ODBCLib::CTransaction>(new CTransaction(m_connectionHandle));
}

std::shared_ptr<ODBCLib::CODBCStatement> ODBCLib::CODBCSession::createStatement() {
	return std::shared_ptr<CODBCStatement>(new CODBCStatement(m_connectionHandle));
}
std::shared_ptr<ODBCLib::CODBCStatement> ODBCLib::CODBCSession::createStatement(const wchar_t* statement) {
	std::shared_ptr<ODBCLib::CODBCStatement> result(createStatement());
	result->prepare(statement);
	return result;
}

std::wstring ODBCLib::CODBCSession::connectionString() const {
	std::wstringstream oss;

	oss << L"DRIVER=" << m_driverName << ";";
	oss << L"SERVER=" << m_serverName << L";";
	if(m_databaseName.size() > 0) {
		oss << L"Database=" << m_databaseName << L";";
	}
	oss << L"Trusted_Connection=";
	if(m_isTrutedConnection) {
		oss << L"yes";
	} else {
		oss << L"no";
	}

	return oss.str();
}
