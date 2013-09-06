#pragma once

#include <list>
#include <winsock.h>
#include <ws2def.h>

namespace MySock {

typedef std::list<SOCKET> SOCKET_LIST;

struct _udp_start_error {
public:
	_udp_start_error(const ADDRINFOA& addr, int s_e, int b_e):
		addrinfo(addr), socket_error(s_e), bind_error(b_e) {
	}
public:
	ADDRINFOA addrinfo;
	int socket_error;
	int bind_error;
} typedef udp_start_error;
typedef std::list<udp_start_error> UDP_STARTERRORS;

union _MySockAddr {
	SOCKADDR_IN v4;
	SOCKADDR_IN6 v6;
} typedef MySockAddr;

}
