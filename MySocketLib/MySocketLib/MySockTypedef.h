#pragma once

#include <list>
#include <WinSock2.h>
#include <ws2def.h>
#include <ws2ipdef.h>

namespace MySock {

typedef std::list<SOCKET> SOCKET_LIST;

union _MySockAddr {
	SOCKADDR addr;
	SOCKADDR_IN v4;
	SOCKADDR_IN6 v6;
} typedef MySockAddr;

}
