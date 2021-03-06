#include "stdafx.h"
#include "ODBCStatement.h"

#include "StatementHandle.h"
#include "DiagInfo.h"

ODBCLib::CODBCStatement::CODBCStatement(std::shared_ptr<CConnectionHandle> connectionHandle):
	m_statementHandle(new CStatementHandle(connectionHandle)) {
}

ODBCLib::CODBCStatement::~CODBCStatement() {
}

bool ODBCLib::CODBCStatement::prepare(const wchar_t* statement) {
	bool result = false;
	int statementLength = wcslen(statement);
	std::vector<wchar_t> statementCopy(statementLength + 1, L'\0');
	std::copy(statement, statement + statementLength, statementCopy.begin());
	SQLRETURN ret = m_statementHandle->prepare(&(*statementCopy.begin()));
	if(ret == SQL_SUCCESS) {
		result = true;
	} else {
		std::wcerr << L"CODBCSession::prepare() CStatementHandle::prepare()=" << ret << std::endl <<
			ODBCLib::CDiagInfo(m_statementHandle).description() << std::endl;
	}
	return  result;
}

std::shared_ptr<ODBCLib::CStatementResult> ODBCLib::CODBCStatement::execute() {
	return std::shared_ptr<ODBCLib::CStatementResult>(new CStatementResult(m_statementHandle, m_statementHandle->execute()));
}

std::shared_ptr<ODBCLib::CStatementResult> ODBCLib::CODBCStatement::execute(const wchar_t* statement) {
	int statementLength = wcslen(statement);
	std::vector<wchar_t> statementCopy(statementLength + 1, L'\0');
	std::copy(statement, statement + statementLength, statementCopy.begin());
	return std::shared_ptr<ODBCLib::CStatementResult>(new CStatementResult(m_statementHandle, m_statementHandle->execute(&(*statementCopy.begin()))));
}

std::shared_ptr<ODBCLib::CStatementResult> ODBCLib::CODBCStatement::nextResult() {
	return std::shared_ptr<ODBCLib::CStatementResult>(new CStatementResult(m_statementHandle, m_statementHandle->nextResult()));
}

bool ODBCLib::CODBCStatement::bindOutputParameter(int idx, int* param, SQLINTEGER* lenOrInd) {
	return (m_statementHandle->BindOutputParameter(idx, param, lenOrInd) == SQL_SUCCESS);
}
bool ODBCLib::CODBCStatement::bindParameter(int idx, int* param, SQLINTEGER* lenOrInd) {
	return (m_statementHandle->BindParameter(idx, param, lenOrInd) == SQL_SUCCESS);
}

void ODBCLib::CODBCStatement::setCursorScrollable(SQLULEN scrollable) {
	m_statementHandle->SetCursorScrollable(scrollable);
}

void ODBCLib::CODBCStatement::setCursorSensitivity(SQLULEN sensitivity) {
	m_statementHandle->SetCursorSensitivity(sensitivity);
}