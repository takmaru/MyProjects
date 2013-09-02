#pragma once

namespace ODBCLib {

class CODBCLibException : public std::exception {
public:
	CODBCLibException();
	explicit CODBCLibException(const char* what);
	virtual ~CODBCLibException();
};

}
