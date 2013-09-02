#include "stdafx.h"
#include "MyException.h"

MyLib::Exception::CMyException::CMyException():
	std::exception(), m_what() {
}
MyLib::Exception::CMyException::~CMyException(){}

void MyLib::Exception::CMyException::setMessage(const char* szFileName, int nLine, const char* szMessage, ...) {

	std::string message = szMessage;

	va_list args = {0};
    va_start(args, szMessage);
	int messageLength = (_vscprintf(szMessage, args) + 1) * sizeof(char);
	if(messageLength > 0) {
		std::vector<char> messageBuffer(messageLength, 0);
		if(_vsnprintf_s(&messageBuffer[0], messageBuffer.size(), _TRUNCATE, szMessage, args) > 0) {
			message = (char*)&messageBuffer[0];
		}
	}
    va_end(args);

	std::ostringstream oss;
	oss << "[" << (const char*)(strrchr(szFileName, '\\') + 1) << ":" << nLine << "]" << message;
	m_what = oss.str();
}