#pragma once

#include "ODBCHandle.h"
#include "ODBCLibDef.h"

namespace ODBCLib {

class CConnectionHandle;

class CStatementHandle : public CODBCHandle {
// constructor & destructor
public:
	explicit CStatementHandle(std::shared_ptr<CConnectionHandle> connectionHandle);
	~CStatementHandle();

// operator
public:
	operator SQLHSTMT() const {
		return static_cast<SQLHSTMT>(m_handle);
	};

// public method
public:
	SQLRETURN prepare(SQLWCHAR* statement);

	SQLRETURN execute();
	SQLRETURN nextResult();

	SQLRETURN BindOutputParameter(SQLUSMALLINT index, int* param, SQLINTEGER* lenOrInd);
	SQLRETURN BindParameter(SQLUSMALLINT index, int* param, SQLINTEGER* lenOrInd);
	SQLRETURN BindParameter(SQLUSMALLINT index, wchar_t* param, SQLSMALLINT paramSize, SQLINTEGER* lenOrInd);
	SQLRETURN BindParameter(SQLUSMALLINT index, __int64* param, SQLINTEGER* lenOrInd);

	SQLSMALLINT resultColCount();
	SQLLEN colAttribute(SQLUSMALLINT col, SQLUSMALLINT fieldID);
	std::wstring colAttribute_String(SQLUSMALLINT col, SQLUSMALLINT fieldID);

	SQLLEN resultRowCount();
	bool isNoCount();

	void SetFetchCount(SQLULEN fetchCount);
	void SetColWiseBind();
	void SetFetchedCountPtr(SQLULEN* fetchedCount);
	void SetRowStatusArray(RowStatusArray& rowStatusArray);
	SQLRETURN BindCol(SQLUSMALLINT col, SQLSMALLINT type, SQLPOINTER valuePtr, SQLLEN elementSize, SQLINTEGER* len);
	SQLRETURN Fetch();

	bool SetRowWiseBinding(int rowLen, int rowCount, SQLUSMALLINT* pStatusArray, SQLULEN* pProcCount);

};	// end of... class CStatementHandle

}	// end of... namespace ODBCLib
