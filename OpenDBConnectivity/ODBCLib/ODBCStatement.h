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
	std::shared_ptr<CStatementResult> nextResult();

private:
	std::shared_ptr<CStatementHandle> m_statementHandle;
};

}
