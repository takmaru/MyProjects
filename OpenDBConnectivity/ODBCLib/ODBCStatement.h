#pragma once

#include "StatementResult.h"

namespace ODBCLib {

class CConnectionHandle;
class CStatementHandle;

class CODBCStatement {
public:
	explicit CODBCStatement(std::shared_ptr<CConnectionHandle> connectionHandle);
	~CODBCStatement();

public:
	bool prepare(const wchar_t* statement);

	std::shared_ptr<CStatementResult> execute();
	std::shared_ptr<CStatementResult> execute(const wchar_t* statement);
	std::shared_ptr<CStatementResult> nextResult();

	// ‚à‚¤‚¿‚å‚Á‚Æ‰ü‘P—\’è
	bool bindOutputParameter(int idx, int* param, SQLINTEGER* lenOrInd);
	bool bindParameter(int idx, int* param, SQLINTEGER* lenOrInd);

	void setCursorScrollable(SQLULEN scrollable);
	void setCursorSensitivity(SQLULEN sensitivity);

private:
	std::shared_ptr<CStatementHandle> m_statementHandle;
};

}
