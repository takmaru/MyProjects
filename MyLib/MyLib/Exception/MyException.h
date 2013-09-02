#pragma once

#include <exception>
#include <string>

namespace MyLib {

namespace Exception {

#define	RAISE_MYEXCEPTION(message, ...)								\
	{																\
		MyLib::Exception::CMyException e;							\
		e.setMessage(__FILE__, __LINE__, message, ##__VA_ARGS__);	\
		throw e;													\
	}

class CMyException : public std::exception {
public:
	CMyException();
	virtual	~CMyException();
	virtual const char* what() const {return m_what.c_str();};
	void setMessage(const char* szFileName, int nLine, const char* szMessage, ...);
private:
	std::string m_what;
};

}
}
