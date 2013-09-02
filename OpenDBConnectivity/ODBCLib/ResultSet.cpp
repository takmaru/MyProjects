#include "stdafx.h"
#include "ResultSet.h"

#include "StatementHandle.h"
#include "ResultColumn.h"
#include "DiagInfo.h"

/*
ODBCLib::CBindColumn::CBindColumn(const ColumnInfo& columnInfo):
	m_columnInfo(columnInfo), m_columnBytes(0), m_bindRowCount(0), m_buffer(), m_bindLength() {
	// ��T�C�Y����
	m_columnBytes = columnInfo.bytes;
	switch(m_columnInfo.type) {
	case SQL_CHAR:
	case SQL_VARCHAR:
		m_columnBytes += sizeof(char);
		break;
	case SQL_WCHAR:
	case SQL_WVARCHAR:
		m_columnBytes += sizeof(wchar_t);
		break;
	}
}
ODBCLib::CBindColumn::~CBindColumn() {
}

void ODBCLib::CBindColumn::Bind(int rowCount, ODBCLib::CStatementHandle& statementHandle, SQLUSMALLINT col) {
	// �o�b�t�@������
	m_bindRowCount = rowCount;
	m_buffer.clear();
	m_buffer.resize((m_bindRowCount) * m_columnBytes, 0);
	m_bindLength.clear();
	m_bindLength.resize(m_bindRowCount, 0);

	// �o�C���h
	SQLRETURN ret = statementHandle.BindCol(col, m_columnInfo.type, &(*m_buffer.begin()), m_columnBytes, &(*m_bindLength.begin()));
	if(ret != SQL_SUCCESS) {
		std::wcerr << L"CBindColumn::Bind() CStatementHandle::BindCol() error(" << ret << L")" << std::endl <<
			ODBCLib::CErrorInfo(SQL_HANDLE_STMT, (SQLHSTMT)statementHandle).description() << std::endl;
	}
}

std::wstring ODBCLib::CBindColumn::description_column() const {
	std::wostringstream oss;
	oss <<
		L"name[" << m_columnInfo.name << L"] " <<
		L"type[" << m_columnInfo.type << L":" << m_columnInfo.typeName << L"] " <<
		L"len=" << m_columnInfo.length << L" " <<
		L"bytes=" << m_columnInfo.bytes;
	return oss.str();
}

std::wstring ODBCLib::CBindColumn::description_value(int idx) const {
	std::wostringstream oss;

	if(idx < m_bindRowCount) {
		const unsigned char* value = GetValuePtr(idx);
		switch(columnType()) {
		case SQL_INTEGER:	oss << *((int*)value);	break;
		case SQL_SMALLINT:	oss << *((short*)value);	break;
		case SQL_BIGINT:	oss << *((__int64*)value);	break;
		case SQL_CHAR:
		case SQL_VARCHAR:	oss << (char*)value;	break;
		case SQL_WCHAR:
		case SQL_WVARCHAR:	oss << (wchar_t*)value;	break;
//		case SQL_DATETIME:	oss << ODBCLib::Datetime2String(*it);	break;
		}
		oss << L"(" << m_bindLength[idx] << L")";
	}

	return oss.str();
}
*/

ODBCLib::CResultSet::CResultSet(std::shared_ptr<CStatementHandle> statementHandle):
	m_statementHandle(statementHandle), m_columns(), m_rowBytes(0), m_maxColByte(0),
	m_rowCount(0), m_rowStatuses() {

	SQLSMALLINT resultColCount = m_statementHandle->resultColCount();
	if(resultColCount > 0) {
		m_columns.reserve(resultColCount);
		for(SQLSMALLINT i = 1; i <= resultColCount; i++) {
			// ����쐬
			CResultColumn column(i, m_statementHandle);
			// ��s�̃o�C�g���A�񒆂̍ő�o�C�g���擾
			m_rowBytes += column.bytes();
			m_maxColByte = max(m_maxColByte, column.bytes());
			// ��������X�g�֒ǉ�
			m_columns.push_back(CResultColumn(i, m_statementHandle));
		}
	}
}

ODBCLib::CResultSet::~CResultSet() {
}

SQLRETURN ODBCLib::CResultSet::Fetch(int rowCount/*= 0*/) {
/*
	// Fetch�s�����m�肷��
	if(rowCount <= 0) {
		// �S�̂̍ő�o�b�t�@�T�C�Y�A�񖈂̍ő�o�b�t�@�T�C�Y���璴���Ȃ��͈͂̍s�����擾
		// �i��s�Œ����Ă��܂��ꍇ�͈�s�����j
		rowCount = min(max(min(MaxBufferSize / m_rowBytes, MaxColumnBufferSize / m_colBytesMax), 1), AutoRowCountMax);
	}

	// Fetch����s�����Z�b�g
	m_statementHandle.SetFetchCount((SQLULEN)rowCount);
	// ��o�C���h�ɃZ�b�g
	m_statementHandle.SetColWiseBind();
	// Fetch�����s�����󂯎��|�C���^���Z�b�g
	m_rowCount = 0;
	m_statementHandle.SetFetchedCountPtr(&m_rowCount);
	// �s�X�e�[�^�X���󂯎��z����Z�b�g
	m_rowStatuses.clear();
	m_rowStatuses.resize(rowCount, SQL_ROW_NOROW);
	m_statementHandle.SetRowStatusArray(m_rowStatuses);

	// �s����n���A�X�e�[�g�����g�n���h���Ƀo�C���h����
	SQLUSMALLINT col = 1;
	BindColumns::iterator it;
	for(it = m_bindColumns.begin(); it != m_bindColumns.end(); ++it) {
		it->Bind(rowCount, m_statementHandle, col);
		col++;
	}

	// Fetch
	return m_statementHandle.Fetch();
*/
	return 0;
}

std::wstring ODBCLib::CResultSet::description() const {
	std::wostringstream oss;

	oss <<	L"���ʃZ�b�g ��=" << m_columns.size() <<
			L" ��s�̃o�b�t�@�T�C�Y=" << m_rowBytes <<
			L" ���̍ő�o�b�t�@�T�C�Y=" << m_maxColByte << std::endl;
	int idx = 0;
	ResultColumns::const_iterator it;
	for(it = m_columns.begin(); it != m_columns.end(); ++it) {
		if(idx > 0) {
			oss << std::endl;
		}
	
		oss << std::right << std::setw(6) << idx << std::left << L" : " <<
			it->description();

		idx++;
	}
	return oss.str();
}

std::wstring ODBCLib::CResultSet::description_resultset() const {
	std::wostringstream oss;

	oss << L"���ʃZ�b�g �o�b�t�@����=" << m_rowStatuses.size() << std::endl;

	bool isOutput = false;
	SQLULEN idx = 0;
	RowStatusArray::const_iterator it;
	for(it = m_rowStatuses.begin(); it != m_rowStatuses.end(); ++it) {
		if((*it) != SQL_ROW_NOROW) {
			if(isOutput) {
				oss << std::endl;
			}
			oss << idx << L" : " << row2str(idx);
			isOutput = true;
		}
		idx++;
	}

	return oss.str();
}

std::wstring ODBCLib::CResultSet::row2str(SQLULEN idx) const {
	std::wostringstream oss;
/*
	BindColumns::const_iterator it;
	for(it = m_bindColumns.begin(); it != m_bindColumns.end(); ++it) {
		if(it != m_bindColumns.begin()) {
			oss << L" | ";
		}
		oss << it->description_value(idx);
	}
*/
	return oss.str();
}
