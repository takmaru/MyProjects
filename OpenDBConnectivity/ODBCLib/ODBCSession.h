#pragma once

#include "Transaction.h"
#include "ODBCStatement.h"

namespace ODBCLib {

class CEnvironmentHandle;
class CConnectionHandle;

class CODBCSession {
// constructor, destructor
public:
	CODBCSession();
	~CODBCSession();

// public method
public:
	bool startSession();
	bool endSession();

	std::shared_ptr<CTransaction> beginTransaction();
	std::shared_ptr<CODBCStatement> createStatement();
	std::shared_ptr<CODBCStatement> createStatement(const wchar_t* statement);

// setter, getter
public:
	bool isConnecting() const {
		return static_cast<bool>(m_connectionHandle);
	};

	void setODBCVersion(SQLSMALLINT version) {
		m_ODBCVersion = version;
	};
	SQLSMALLINT odbcVersion() const {
		return m_ODBCVersion;
	};

	void setDriverName(const wchar_t* driverName) {
		m_driverName = driverName;
	};
	void setServerName(const wchar_t* serverName) {
		m_serverName = serverName;
	};
	void setDatabaseName(const wchar_t* databaseName) {
		m_databaseName = databaseName;
	};
	void setTrutedConnection(bool isTruted) {
		m_isTrutedConnection = isTruted;
	};
	std::wstring connectionString() const;

private:
	bool m_isConnecting;

	std::shared_ptr<CEnvironmentHandle> m_environmentHandle;
	SQLSMALLINT m_ODBCVersion;

	std::shared_ptr<CConnectionHandle> m_connectionHandle;
	std::wstring m_driverName;
	std::wstring m_serverName;
	std::wstring m_databaseName;
	bool m_isTrutedConnection;
};

}
