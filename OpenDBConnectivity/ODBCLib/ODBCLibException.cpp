#include "stdafx.h"
#include "ODBCLibException.h"

ODBCLib::CODBCLibException::CODBCLibException() : std::exception("ODBCLibException") {
}
ODBCLib::CODBCLibException::CODBCLibException(const char* what) : std::exception(what) {
}
ODBCLib::CODBCLibException::~CODBCLibException() {
}
