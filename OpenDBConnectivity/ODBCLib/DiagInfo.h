#pragma once

//#include "DiagRecord.h"

namespace ODBCLib {

class CODBCHandle;
class CDiagRecord;

class CDiagInfo {
private:
	typedef std::vector<std::unique_ptr<CDiagRecord> > DiagRecords;

// constructor & destructor
public:
	explicit CDiagInfo(std::shared_ptr<CODBCHandle> handle);
	~CDiagInfo();

// public const method
public:
	std::wstring description() const;

// private method
private:
	int recordCount() const {
		return m_records.size();
	}

// private member
private:
	SQLSMALLINT m_type;
	SQLHANDLE m_handle;

	DiagRecords m_records;
};	// end of... class CStatementHandle

};	// end of... namespace ODBCLib
