#include "stdafx.h"
#include "TCPSocket.h"

#include <sstream>

#include "MySockException.h"

MySock::CTCPSocket::CTCPSocket() : CSocketBase() {
}
MySock::CTCPSocket::CTCPSocket(const CTCPSocket& obj) : CSocketBase(obj) {
}
MySock::CTCPSocket::CTCPSocket(SOCKET sock, int family) : CSocketBase(sock, family) {
}

MySock::CTCPSocket::~CTCPSocket() {
}

void MySock::CTCPSocket::create(int family) {
	CSocketBase::create_socket(family, SOCK_STREAM, IPPROTO_TCP);
}

void MySock::CTCPSocket::listen() {
	// check
	if(m_sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[listen] socket isn't created!!");
	}

	// listen
	if(::listen(m_sock, SOMAXCONN) != 0) {
		RAISE_MYSOCKEXCEPTION("[listen] listen err=%d", ::WSAGetLastError());
	}
}

MySock::CTCPSocket MySock::CTCPSocket::accept(MySock::MySockAddr* sockaddr/*= NULL*/) {
	// check
	if(m_sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[accept] socket isn't created!!");
	}

	// accept
	MySockAddr mySockAddr = {0};
	int sockaddrlen = sizeof(mySockAddr.v4);
	if(m_family == AF_INET6) {
		sockaddrlen = sizeof(mySockAddr.v6);
	}
	SOCKET sock = ::accept(m_sock, &mySockAddr.addr, &sockaddrlen);
	if(sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[accept] accept err=%d", ::WSAGetLastError());
	}

	return MySock::CTCPSocket(sock, m_family);
}
