#pragma once

namespace ODBCLib {

class CODBCHandle {
public:
	explicit CODBCHandle(SQLSMALLINT type);
	virtual ~CODBCHandle();

public:
	SQLSMALLINT type() const {
		return m_type;
	};
	SQLHANDLE handle() const {
		return m_handle;
	};

	bool isHandleEnable() const {
		return (m_handle != SQL_NULL_HANDLE);
	};

protected:
	void FreeHandle();

protected:
	SQLHANDLE m_handle;

private:
	SQLSMALLINT m_type;
};

}