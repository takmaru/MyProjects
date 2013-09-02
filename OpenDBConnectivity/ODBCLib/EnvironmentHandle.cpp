#include "stdafx.h"
#include "EnvironmentHandle.h"


ODBCLib::CEnvironmentHandle::CEnvironmentHandle() : CODBCHandle(SQL_HANDLE_ENV) {
	// �n���h���쐬
	::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_handle);
}
ODBCLib::CEnvironmentHandle::~CEnvironmentHandle() {
}

// �����ݒ�
SQLRETURN ODBCLib::CEnvironmentHandle::setVersion(SQLSMALLINT version) {
	return ::SQLSetEnvAttr(static_cast<SQLHENV>(m_handle), SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(version), 0);
}
