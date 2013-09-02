#include "stdafx.h"
#include "ODBCHandle.h"

ODBCLib::CODBCHandle::CODBCHandle(SQLSMALLINT type) : m_type(type), m_handle(SQL_NULL_HANDLE) {
}

ODBCLib::CODBCHandle::~CODBCHandle() {
	FreeHandle();
}

void ODBCLib::CODBCHandle::FreeHandle() {
	if(m_handle != SQL_NULL_HANDLE) {
		::SQLFreeHandle(m_type, m_handle);
		m_handle = SQL_NULL_HANDLE;
	}
}