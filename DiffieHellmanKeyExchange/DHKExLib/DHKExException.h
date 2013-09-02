#pragma once

#include <MyLib/Exception/MyException.h>

namespace DHKEx {

#define	RAISE_DHKExEXCEPTION(message, ...)							\
	{																\
		CDHExException e;											\
		e.setMessage(__FILE__, __LINE__, message, ##__VA_ARGS__);	\
		throw e;													\
	}

typedef MyLib::Exception::CMyException CDHExException;

}
