#include "stdafx.h"
#include "DiagRecord.h"

#include "ODBCHandle.h"
#include "ODBCLibUtil.h"

ODBCLib::CDiagRecord::CDiagRecord(std::shared_ptr<CODBCHandle> handle, SQLSMALLINT idx):
	m_idx(idx), m_code(0), m_message() {

	// sqlstate
	ODBCLib::GetDiagFieldInfo_String(handle->type(), handle->handle(), m_idx, SQL_DIAG_SQLSTATE, m_sqlstate, sizeof(m_sqlstate));
	// code
	ODBCLib::GetDiagFieldInfo_Value(handle->type(), handle->handle(), m_idx, SQL_DIAG_NATIVE, &m_code, SQL_IS_INTEGER);
	// message
	ODBCLib::GetDiagFieldInfo_String(handle->type(), handle->handle(), m_idx, SQL_DIAG_MESSAGE_TEXT, m_message);
}

ODBCLib::CDiagRecord::~CDiagRecord() {
}

std::wstring ODBCLib::CDiagRecord::description() const {
	std::wostringstream oss;
	oss <<	m_idx << L" : " <<
			descriptionInfo();
	return oss.str();
}
std::wstring ODBCLib::CDiagRecord::descriptionInfo() const {
	std::wostringstream oss;
	oss <<	L"[" << m_sqlstate << L"]" <<
			L"[" << m_code << L"]" <<
			m_message;
	return oss.str();
}
