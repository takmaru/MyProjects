#include "stdafx.h"
#include "ResultRowCount.h"

#include "StatementHandle.h"


ODBCLib::CResultRowCount::CResultRowCount(std::shared_ptr<CStatementHandle> statementHandle) :
	m_rowCount(0), m_isNoCount(false) {
	m_rowCount = statementHandle->resultRowCount();
	if(m_rowCount == 0) {
		m_isNoCount = statementHandle->isNoCount();
	}
}

ODBCLib::CResultRowCount::~CResultRowCount() {
}

SQLLEN ODBCLib::CResultRowCount::rowCount(bool* isNoCount/*= NULL*/) const {
	if(isNoCount != NULL) {
		*isNoCount = false;
		if(m_rowCount == 0) {
			*isNoCount = m_isNoCount;
		}
	}
	return m_rowCount;
};
