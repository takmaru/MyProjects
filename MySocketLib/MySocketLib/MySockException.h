#pragma once

#include <MyLib/Exception/MyException.h>

namespace MySock {

#define	RAISE_MYSOCKEXCEPTION(message, ...)							\
	{																\
		CMySockException e;											\
		e.setMessage(__FILE__, __LINE__, message, ##__VA_ARGS__);	\
		throw e;													\
	}

typedef MyLib::Exception::CMyException CMySockException;

}
