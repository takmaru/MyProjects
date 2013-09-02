#include "stdafx.h"
#include "DiagInfo.h"

#include "ODBCHandle.h"
#include "DiagRecord.h"
#include "ODBCLibUtil.h"

ODBCLib::CDiagInfo::CDiagInfo(std::shared_ptr<ODBCLib::CODBCHandle> handle):
	m_type(handle->type()), m_handle(handle->handle()), m_records() {

	// レコード数取得
	SQLINTEGER recordCount = 0;
	ODBCLib::GetDiagFieldInfo_Value(m_type, m_handle, 0, SQL_DIAG_NUMBER, &recordCount, SQL_IS_SMALLINT);
	// 詳細情報取得ループ
	m_records.reserve(recordCount);
	for(SQLINTEGER i = 1; i <= recordCount; i++) {
		m_records.push_back(DiagRecords::value_type(new CDiagRecord(handle, static_cast<SQLSMALLINT>(i))));
	}
}
ODBCLib::CDiagInfo::~CDiagInfo() {
}

std::wstring ODBCLib::CDiagInfo::description() const {
	std::wostringstream oss;
	DiagRecords::const_iterator it;
	for(it = m_records.begin(); it != m_records.end(); ++it) {
		if(it != m_records.begin()) {
			oss << std::endl;
		}
		oss << (*it)->description();
	}
	return oss.str();
}
/*
ODBCLib::CDiagInfo::FieldInfo ODBCLib::CDiagInfo::GetDiagFieldInfo(SQLSMALLINT record) const {
	SQLRETURN ret = SQL_SUCCESS;
	ODBCLib::CDiagInfo::FieldInfo field;

	// rowNumber
	ret = ::SQLGetDiagFieldW(m_type, m_handle, record, SQL_DIAG_ROW_NUMBER, &field.rowNumber, SQL_IS_INTEGER, NULL);
	if(ret != SQL_SUCCESS) {
		std::wcerr << L"CDiagInfo::GetFieldInfo() SQL_DIAG_ROW_NUMBER SQLGetDiagFieldW error(" << ret << L")" << std::endl;
	}
	// line
	ret = ::SQLGetDiagFieldW(m_type, m_handle, record, SQL_DIAG_SS_LINE, &field.line, SQL_IS_USMALLINT, NULL);
	if(ret != SQL_SUCCESS) {
		std::wcerr << L"CDiagInfo::GetFieldInfo() SQL_DIAG_SS_LINE SQLGetDiagFieldW error(" << ret << L")" << std::endl;
	}
	// msgState
	ret = ::SQLGetDiagFieldW(m_type, m_handle, record, SQL_DIAG_SS_MSGSTATE, &field.msgState, SQL_IS_INTEGER, NULL);
	if(ret != SQL_SUCCESS) {
		std::wcerr << L"CDiagInfo::GetFieldInfo() SQL_DIAG_SS_MSGSTATE SQLGetDiagFieldW error(" << ret << L")" << std::endl;
	}
	// severity
	ret = ::SQLGetDiagFieldW(m_type, m_handle, record, SQL_DIAG_SS_SEVERITY, &field.severity, SQL_IS_INTEGER, NULL);
	if(ret != SQL_SUCCESS) {
		std::wcerr << L"CDiagInfo::GetFieldInfo() SQL_DIAG_SS_SEVERITY SQLGetDiagFieldW error(" << ret << L")" << std::endl;
	}
	// procName
	field.procName = GetDiagFieldInfo_String(record, SQL_DIAG_SS_PROCNAME);
	// serverName
	field.serverName = GetDiagFieldInfo_String(record, SQL_DIAG_SS_SRVNAME);

	return field;
}

std::wstring ODBCLib::CDiagInfo::GetDiagFieldInfo_String(SQLSMALLINT record, SQLSMALLINT diagId) const {

	std::wstring fieldStr;

	SQLRETURN ret = SQL_SUCCESS;
	SQLSMALLINT bytes = 0;
	std::vector<unsigned char> fieldStrBuffer;
	ret = ::SQLGetDiagFieldW(m_type, m_handle, record, diagId, NULL, 0, &bytes);
	if(ret == SQL_SUCCESS) {
		if(bytes > 0) {
			// SQL_SUCCESSでデータ長が返ってきていれば、文字列取得
			bytes += sizeof(wchar_t);	// NULL文字分、追加
			fieldStrBuffer.resize(bytes, L'\0');
			ret = ::SQLGetDiagFieldW(m_type, m_handle, record, diagId, &(*fieldStrBuffer.begin()), bytes, &bytes);
			if(ret == SQL_SUCCESS) {
				fieldStr = (wchar_t*)(&(*fieldStrBuffer.begin()));
			} else {
				std::wcerr << L"CDiagInfo::GetDiagFieldInfo_String() SQLGetDiagFieldW(ID:" << diagId << ") error(" << ret << L")" << std::endl;
			}
		}
	} else {
		// getLength error
		std::wcerr << L"CDiagInfo::GetDiagFieldInfo_String() SQLGetDiagFieldW(" << diagId << ") getLength error(" << ret << L")" << std::endl;
	}

	return fieldStr;
}
*/