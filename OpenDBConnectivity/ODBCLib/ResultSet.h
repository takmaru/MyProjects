#pragma once

#include "ODBCLibDef.h"

namespace ODBCLib {

class CStatementHandle;

struct _ColumnInfo {
public:
	std::wstring name;
/*
	std::wstring columnName;
	std::wstring tableName;
	std::wstring baseTableName;
	std::wstring schemaName;
	std::wstring catalogName;
	std::wstring label;

	std::wstring literalPrefix;
	std::wstring literalSuffix;
*/
	SQLSMALLINT type;
	std::wstring typeName;
/*
	SQLINTEGER conciseType;
	std::wstring localTypeName;
*/
	SQLINTEGER length;
	SQLINTEGER bytes;
/*
	SQLINTEGER displaySize;

	BOOL isAutoUniqueValue;
	BOOL isCaseSesitive;
	BOOL isFixedPrecScale;
	BOOL isNullable;
	BOOL isUnsigned;

	SQLINTEGER numPrecRadix;
	SQLINTEGER precision;
	SQLINTEGER scale;
	SQLINTEGER searchable;
	SQLINTEGER unnamed;
	SQLINTEGER updatable;
*/
} typedef ColumnInfo;
/*
class CBindColumn {
private:
	typedef std::vector<unsigned char> BindBuffer;
	typedef std::vector<SQLLEN> BindLength;

public:
	explicit CBindColumn(const ColumnInfo& columnInfo);
	~CBindColumn();

	void Bind(int rowCount, CStatementHandle& statementHandle, SQLUSMALLINT col);

	unsigned char* GetBufferPtr() {
		return &(*m_buffer.begin());
	};
	SQLLEN* GetBufferLengthPtr() {
		return &(*m_bindLength.begin());
	};

	SQLSMALLINT columnType() const {
		return m_columnInfo.type;
	};

	const unsigned char* GetValuePtr(int idx) const {
		if(idx >= m_bindRowCount) {
			return NULL;
		}
		return &m_buffer[idx * m_columnBytes];
	}

	std::wstring description_column() const;
	std::wstring description_value(int idx) const;

private:
	ColumnInfo m_columnInfo;

	int m_columnBytes;
	int m_bindRowCount;
	BindBuffer m_buffer;
	BindLength m_bindLength;
};
*/
class CResultColumn;

class CResultSet {
private:
	static const int MaxBufferSize = 256 * 1024 * 1024;
	static const int MaxColumnBufferSize = 64 * 1024 * 1024;
	static const int AutoRowCountMax = 1000;

public:
	typedef std::vector<CResultColumn> ResultColumns;

// constructor & destructor
public:
	explicit CResultSet(std::shared_ptr<CStatementHandle> statementHandle);
	~CResultSet();

// public method
public:
	SQLRETURN Fetch(int rowCount = 0);

// public const method
public:
	const ResultColumns& columns() {
		return m_columns;
	};

	std::wstring description() const;
	std::wstring description_resultset() const;

private:
	std::wstring row2str(SQLULEN idx) const;

// private member
private:
	std::shared_ptr<CStatementHandle> m_statementHandle;

	ResultColumns m_columns;

	int m_rowBytes;
	int m_maxColByte;

	SQLULEN m_rowCount;
	RowStatusArray m_rowStatuses;
};	// end of... class CResultSet

}	// end of... namespace ODBCLib
