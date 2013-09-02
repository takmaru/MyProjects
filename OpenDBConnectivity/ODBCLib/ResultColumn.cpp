#include "stdafx.h"
#include "ResultColumn.h"

#include "StatementHandle.h"

ODBCLib::CResultColumn::CResultColumn(SQLSMALLINT idx, std::shared_ptr<CStatementHandle> statementHandle):
	m_idx(idx), m_name(), m_type(), m_typeName(), m_length(0), m_bytes(0) {

	m_name = statementHandle->colAttribute_String(m_idx, SQL_DESC_NAME);
	m_type = static_cast<SQLSMALLINT>(statementHandle->colAttribute(m_idx, SQL_DESC_TYPE));
	m_typeName = statementHandle->colAttribute_String(m_idx, SQL_DESC_TYPE_NAME);
	m_length = statementHandle->colAttribute(m_idx, SQL_DESC_LENGTH);
	m_bytes = statementHandle->colAttribute(m_idx, SQL_DESC_OCTET_LENGTH);
}

ODBCLib::CResultColumn::~CResultColumn() {
}

SQLINTEGER ODBCLib::CResultColumn::bytes() const {
	SQLINTEGER retBytes = m_bytes;
	switch(m_type) {
	case SQL_CHAR:
	case SQL_VARCHAR:
		retBytes += sizeof(char);
		break;
	case SQL_WCHAR:
	case SQL_WVARCHAR:
		retBytes += sizeof(wchar_t);
		break;
	}
	return retBytes;
}

std::wstring ODBCLib::CResultColumn::description() const {
	std::wostringstream oss;
	oss <<
		L"name[" << m_name << L"] " <<
		L"type[" << m_type << L":" << m_typeName << L"] " <<
		L"len=" << m_length << L" " <<
		L"bytes=" << m_bytes;
	return oss.str();
}
