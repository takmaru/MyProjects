#pragma once

#include <list>
#include <winsock.h>
#include <ws2def.h>
#include <ws2ipdef.h>

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

struct _udp_bind_error {
public:
	_udp_bind_error(const ADDRINFOA& addr, int s_e, int b_e):
		addrinfo(addr), socket_error(s_e), bind_error(b_e) {
	}
public:
	ADDRINFOA addrinfo;
	int socket_error;
	int bind_error;
} typedef udp_bind_error;
typedef std::list<udp_bind_error> UDP_BINDERRORS;

enum MySockErrorType {
	Invalid = 0,
	socket,
	bind,
	send,
	less_sendlen
};
struct _mysock_error_info {
public:
	_mysock_error_info(MySockErrorType errType, int err, const ADDRINFOA& addr):
		type(errType), error(err), addrinfo(addr) {
	}
public:
	MySockErrorType type;
	int error;
	ADDRINFOA addrinfo;
} typedef mysock_error_info;
typedef std::list<mysock_error_info> MYSOCKERRORS;

union _MySockAddr {
	SOCKADDR_IN v4;
	SOCKADDR_IN6 v6;
} typedef MySockAddr;

}
