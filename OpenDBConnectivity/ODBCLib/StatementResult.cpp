#include "stdafx.h"
#include "StatementResult.h"

#include "StatementHandle.h"
#include "ResultSet.h"
#include "ResultColumn.h"
#include "ResultRowCount.h"
#include "DiagInfo.h"

ODBCLib::CStatementResult::CStatementResult(std::shared_ptr<CStatementHandle> statementHandle, SQLRETURN result):
	m_statementHandle(statementHandle), m_result(result), m_resultSet(), m_resultRowCount(), m_diagInfo() {

	if(m_result == SQL_SUCCESS) {
		m_resultSet.reset(new ODBCLib::CResultSet(m_statementHandle));
		if(m_resultSet->columns().size() <= 0) {
			m_resultRowCount.reset(new ODBCLib::CResultRowCount(statementHandle));
		}
	} else if(	(m_result == SQL_SUCCESS_WITH_INFO) ||
				(m_result == SQL_ERROR)	) {
		m_diagInfo.reset(new ODBCLib::CDiagInfo(m_statementHandle));
	}
}

ODBCLib::CStatementResult::~CStatementResult() {
}

ODBCLib::CStatementResult::ResultType ODBCLib::CStatementResult::resultType() const {
	ResultType ret = RT_Error;

	if(	(m_result == SQL_SUCCESS) ||
		(m_result == SQL_SUCCESS_WITH_INFO)	) {

		if(	(m_resultSet.get() != NULL) &&
			(m_resultSet->columns().size() > 0)	) {
			ret = RT_ResultSet;
		} else if(m_resultRowCount.get() != NULL) {
			ret = RT_ResultRowCount;
		} else if(m_diagInfo.get() != NULL) {
			ret = RT_Info;
		} else {
			ret = RT_None;
		}
	}

	return ret;
}