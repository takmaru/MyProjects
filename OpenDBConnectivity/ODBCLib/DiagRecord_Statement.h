#pragma once

#include "DiagRecord.h"

namespace ODBCLib {

class CDiagRecord_Statement : public CDiagRecord {
public:
	CDiagRecord_Statement(std::shared_ptr<CODBCHandle> handle, SQLSMALLINT idx);
	~CDiagRecord_Statement();

public:
	SSHORT line() const {
		return m_line;
	};
	SQLINTEGER severity() const {
		return m_severity;
	};
	SQLINTEGER state() const {
		return m_state;
	};

private:
	std::wstring descriptionInfo() const;

private:
	SSHORT m_line;
	SQLINTEGER m_severity;
	SQLINTEGER m_state;
};

}
