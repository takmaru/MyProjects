#include "stdafx.h"
#include "ConnectionHandle.h"

#include "EnvironmentHandle.h"
#include "DiagInfo.h"

ODBCLib::CConnectionHandle::CConnectionHandle(std::shared_ptr<CEnvironmentHandle> environmentHandle) : CODBCHandle(SQL_HANDLE_DBC) {
	// ハンドル作成
	SQLRETURN ret = ::SQLAllocHandle(SQL_HANDLE_DBC, environmentHandle->handle(), &m_handle);
	if(ret != SQL_SUCCESS) {
		std::wcerr << L"CConnectionHandle::CConnectionHandle() SQLAllocHandle()=" << ret << std::endl <<
			ODBCLib::CDiagInfo(environmentHandle).description() << std::endl;
	}
}
ODBCLib::CConnectionHandle::~CConnectionHandle() {
}

SQLRETURN ODBCLib::CConnectionHandle::connect(const SQLWCHAR* connectionString) {
	SQLWCHAR connectionStringCopy[1024] = {0};
	wcscpy_s(connectionStringCopy, connectionString);
	// データベースへ接続
	return ::SQLDriverConnectW(static_cast<SQLHDBC>(m_handle), NULL, connectionStringCopy, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
}
SQLRETURN ODBCLib::CConnectionHandle::disconnect() {
	return ::SQLDisconnect(static_cast<SQLHDBC>(m_handle));
}

// トランザクション開始
SQLRETURN ODBCLib::CConnectionHandle::beginTransaction() {
	return ::SQLSetConnectAttrW(static_cast<SQLHDBC>(m_handle), SQL_ATTR_AUTOCOMMIT, static_cast<SQLPOINTER>(SQL_AUTOCOMMIT_OFF), SQL_IS_UINTEGER);
}
// コミット
SQLRETURN ODBCLib::CConnectionHandle::commit() {
	return ::SQLEndTran(SQL_HANDLE_DBC, m_handle, SQL_COMMIT);
}
// ロールバック
SQLRETURN ODBCLib::CConnectionHandle::rollback() {
	return ::SQLEndTran(SQL_HANDLE_DBC, m_handle, SQL_ROLLBACK);
}

SQLULEN ODBCLib::CConnectionHandle::attributes(SQLINTEGER attr) {
	SQLULEN value = 0;
	SQLRETURN ret = ::SQLGetConnectAttrW(static_cast<SQLHDBC>(m_handle), attr, &value, 0, NULL);
	if(ret != SQL_SUCCESS) {
		std::wcerr << L"CConnectionHandle::attributes() SQLGetConnectAttrW error(" << ret << L")" << std::endl;
	}
	return value;
}