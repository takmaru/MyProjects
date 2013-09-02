#include "stdafx.h"
#include "Transaction.h"

#include "ConnectionHandle.h"
#include "DiagInfo.h"

ODBCLib::CTransaction::CTransaction(std::shared_ptr<ODBCLib::CConnectionHandle> connectionHandle):
	m_connectionHandle(connectionHandle), m_isEnabled(false), m_isEndTransaction(false) {
	SQLRETURN ret = m_connectionHandle->beginTransaction();
	if(ret == SQL_SUCCESS) {
		m_isEnabled = true;
	} else {
		std::wcerr << L"CTransaction::CTransaction() CConnectionHandle::beginTransaction()=" << ret << std::endl <<
			ODBCLib::CDiagInfo(m_connectionHandle).description() << std::endl;
	}
}
ODBCLib::CTransaction::~CTransaction() {
	rollback();
}

bool ODBCLib::CTransaction::commit() {
	if(!m_isEnabled) {
		return false;
	}
	if(m_isEndTransaction) {
		return false;
	}
	m_isEndTransaction = true;

	bool result = false;
	SQLRETURN ret = m_connectionHandle->commit();
	if(ret == SQL_SUCCESS) {
		result = true;
	} else {
		std::wcerr << L"CTransaction::commit() CConnectionHandle::commit()=" << ret << std::endl <<
			ODBCLib::CDiagInfo(m_connectionHandle).description() << std::endl;
	}
	return result;
}

bool ODBCLib::CTransaction::rollback() {
	if(!m_isEnabled) {
		return false;
	}
	m_isEndTransaction = true;

	bool result = false;
	SQLRETURN ret = m_connectionHandle->rollback();
	if(ret == SQL_SUCCESS) {
		result = true;
	} else {
		std::wcerr << L"CTransaction::rollback() CConnectionHandle::rollback()=" << ret << std::endl <<
			ODBCLib::CDiagInfo(m_connectionHandle).description() << std::endl;
	}
	return result;
}
