#pragma once

#include "ODBCHandle.h"

namespace ODBCLib {

class CEnvironmentHandle : public CODBCHandle {
// constructor & destructor
public:
	CEnvironmentHandle();
	~CEnvironmentHandle();

// operator
public:
	operator SQLHENV() const {
		return static_cast<SQLHENV>(m_handle);
	};

// public method
public:
	// SQL_OV_ODBC3, SQL_OV_ODBC2, SQL_OV_ODBC3_80
	SQLRETURN setVersion(SQLSMALLINT version);

};	// end of... class CEnvironmentHandle

}	// end of... namespace ODBCLib
