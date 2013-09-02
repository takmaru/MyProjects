#pragma once

#include "ODBCHandle.h"

namespace ODBCLib {

class CEnvironmentHandle;

class CConnectionHandle : public CODBCHandle {
// constructor & destructor
public:
	explicit CConnectionHandle(std::shared_ptr<CEnvironmentHandle> environmentHandle);
	~CConnectionHandle();

// operator
public:
	operator SQLHDBC() const {
		return static_cast<SQLHDBC>(m_handle);
	};

// public method
public:
	SQLRETURN connect(const SQLWCHAR* connectionString);
	SQLRETURN disconnect();

	SQLRETURN beginTransaction();
	SQLRETURN commit();
	SQLRETURN rollback();

	SQLULEN attributes(SQLINTEGER attr);
};	// end of... class CConnectionHandle

}	// end of... namespace ODBCLib
