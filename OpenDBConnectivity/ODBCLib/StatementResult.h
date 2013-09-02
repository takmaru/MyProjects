#pragma once

namespace ODBCLib {

class CStatementHandle;
class CResultSet;
class CResultRowCount;
class CDiagInfo;

class CStatementResult {
public:
	// ¬Œ÷‚ÌŒ‹‰Êí•Ê
	enum ResultType {
		RT_Error,
		RT_None,
		RT_ResultSet,
		RT_ResultRowCount,
		RT_Info
	};

public:
	CStatementResult(std::shared_ptr<CStatementHandle> statementHandle, SQLRETURN result);
	~CStatementResult();

public:
	ResultType resultType() const;

	SQLRETURN result() const {
		return m_result;
	};

	std::shared_ptr<CResultSet> resultSet() {
		return m_resultSet;
	};
	std::shared_ptr<CResultRowCount> resultRowCount() const {
		return m_resultRowCount;
	};
	const std::shared_ptr<CDiagInfo> diagInfo() const {
		return m_diagInfo;
	};
;
private:
	std::shared_ptr<CStatementHandle> m_statementHandle;
	SQLRETURN m_result;
	std::shared_ptr<CResultSet> m_resultSet;
	std::shared_ptr<CResultRowCount> m_resultRowCount;
	std::shared_ptr<CDiagInfo> m_diagInfo;
};

}
