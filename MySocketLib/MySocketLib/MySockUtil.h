#pragma once

#include <MyLib/tstring/tstring.h>

#include <WinSock2.h>

namespace MySock {
	std::tstring addressToString(const PSOCKADDR sockaddr);
	std::tstring ntop(const PSOCKADDR sockaddr);
}
