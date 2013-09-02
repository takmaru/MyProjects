#pragma once

namespace ODBCLib {

class CConnectionHandle;

class CTransaction {
public:
	explicit CTransaction(std::shared_ptr<CConnectionHandle> connectionHandle);
	~CTransaction();

public:
	bool commit();
	bool rollback();

public:
	bool isEnabled() const {
		return m_isEnabled;
	};
	bool isEndTransaction() const {
		return m_isEndTransaction;
	};

private:
	std::shared_ptr<ODBCLib::CConnectionHandle> m_connectionHandle;

	bool m_isEnabled;
	bool m_isEndTransaction;
};

}
