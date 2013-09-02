#pragma once

namespace ODBCLib {

class CODBCHandle;

class CDiagRecord {
public:
	CDiagRecord(std::shared_ptr<CODBCHandle> handle, SQLSMALLINT idx);
	virtual ~CDiagRecord();

public:
	SQLSMALLINT idx() const {
		return m_idx;
	};
	std::wstring sqlState() const {
		return std::wstring(m_sqlstate);
	};
	SQLINTEGER code() const {
		return m_code;
	};
	std::wstring message() const {
		return m_message;
	};

public:
	std::wstring description() const;
protected:
	virtual std::wstring descriptionInfo() const;

private:
	SQLSMALLINT m_idx;
	SQLSTATE m_sqlstate;
	SQLINTEGER m_code;
	std::wstring m_message;
};

}
