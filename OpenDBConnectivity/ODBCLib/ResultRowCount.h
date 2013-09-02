#pragma once

namespace ODBCLib {

class CStatementHandle;

class CResultRowCount {
public:
	explicit CResultRowCount(std::shared_ptr<CStatementHandle> statementHandle);
	~CResultRowCount();

private:
	SQLLEN rowCount(bool* isNoCount = NULL) const;

private:
	SQLLEN m_rowCount;
	bool m_isNoCount;
};

}
