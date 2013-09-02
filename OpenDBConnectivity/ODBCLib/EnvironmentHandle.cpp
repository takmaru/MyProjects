#include "stdafx.h"
#include "EnvironmentHandle.h"


ODBCLib::CEnvironmentHandle::CEnvironmentHandle() : CODBCHandle(SQL_HANDLE_ENV) {
	// ƒnƒ“ƒhƒ‹ì¬
	::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_handle);
}
ODBCLib::CEnvironmentHandle::~CEnvironmentHandle() {
}

// ‘®«İ’è
SQLRETURN ODBCLib::CEnvironmentHandle::setVersion(SQLSMALLINT version) {
	return ::SQLSetEnvAttr(static_cast<SQLHENV>(m_handle), SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(version), 0);
}
