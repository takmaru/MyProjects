#include "stdafx.h"
#include "DiagRecord_Statement.h"

#include "ODBCHandle.h"
#include "ODBCLibUtil.h"

ODBCLib::CDiagRecord_Statement::CDiagRecord_Statement(std::shared_ptr<CODBCHandle> handle, SQLSMALLINT idx):
	CDiagRecord(handle, idx),
	m_line(0), m_severity(0), m_state(0){

	// line
	ODBCLib::GetDiagFieldInfo_Value(handle->type(), handle->handle(), idx, SQL_DIAG_SS_LINE, &m_line, SQL_IS_USMALLINT);
	// severity
	ODBCLib::GetDiagFieldInfo_Value(handle->type(), handle->handle(), idx, SQL_DIAG_SS_SEVERITY, &m_severity, SQL_IS_INTEGER);
	// state
	ODBCLib::GetDiagFieldInfo_Value(handle->type(), handle->handle(), idx, SQL_DIAG_SS_MSGSTATE, &m_state, SQL_IS_INTEGER);
}

ODBCLib::CDiagRecord_Statement::~CDiagRecord_Statement() {
}

std::wstring ODBCLib::CDiagRecord_Statement::descriptionInfo() const {
	std::wostringstream oss;
	oss <<	L"line=" << m_line << L", " <<
			L"severity=" << m_severity << L", " <<
			L"state=" << m_state << L", " <<
			CDiagRecord::descriptionInfo();
	return oss.str();
}
