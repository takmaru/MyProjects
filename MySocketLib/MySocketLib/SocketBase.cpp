#include "stdafx.h"
#include "SocketBase.h"

#include <sstream>

#include "MySockException.h"

namespace {
	const unsigned int DEFAULT_RECV_BUFFSIZE = 4 * 1024;
	const unsigned int MIN_RECV_BUFFSIZE = 256;
	const unsigned int MAX_RECV_BUFFSIZE = 4 * 1024 * 1024;
}

MySock::CSocketBase::CSocketBase():
	m_sock(INVALID_SOCKET), m_family(AF_UNSPEC), m_mySockAddr(), m_peerSockAddr(),
	m_recvBufferSize(DEFAULT_RECV_BUFFSIZE), m_recvBuffer(m_recvBufferSize, 0) {
	m_mySockAddr.addr.sa_family = AF_UNSPEC;
	m_peerSockAddr.addr.sa_family = AF_UNSPEC;
}

MySock::CSocketBase::CSocketBase(const CSocketBase& obj):
	m_sock(obj.m_sock), m_family(obj.m_family), m_mySockAddr(obj.m_mySockAddr), m_peerSockAddr(obj.m_peerSockAddr),
	m_recvBufferSize(obj.m_recvBufferSize), m_recvBuffer(m_recvBufferSize, 0) {
}

MySock::CSocketBase::CSocketBase(SOCKET sock, int family):
	m_sock(sock), m_family(family), m_mySockAddr(), m_peerSockAddr(),
	m_recvBufferSize(DEFAULT_RECV_BUFFSIZE), m_recvBuffer(m_recvBufferSize, 0) {
	m_mySockAddr.addr.sa_family = AF_UNSPEC;
	m_peerSockAddr.addr.sa_family = AF_UNSPEC;
}

MySock::CSocketBase::~CSocketBase() {
}

void MySock::CSocketBase::create_socket(int family, int type, int protocol) {
	// check
	if(m_sock != INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[create_socket] socket is created!!");
	}

	m_sock = ::socket(family, type, protocol);
	if(m_sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[create_socket] socket err=%d", ::WSAGetLastError());
	}
	m_family = family;
}
void MySock::CSocketBase::close() {
	if(m_sock != INVALID_SOCKET) {
		if(::closesocket(m_sock) != 0) {
			std::tcerr << _T("CSocketBase::close closesocket err=") << ::WSAGetLastError() << std::endl;
		}
		m_sock = INVALID_SOCKET;
	}
}

void MySock::CSocketBase::bind(const MySock::MySockAddr& sockaddr) {
	// check
	if(m_sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[bind] socket isn't created!!");
	}

	int addrlen = 0;
	if(sockaddr.addr.sa_family == AF_INET) {
		addrlen = sizeof(sockaddr.v4);
	} else if(sockaddr.addr.sa_family == AF_INET6) {
		addrlen = sizeof(sockaddr.v6);
	} else {
		RAISE_MYSOCKEXCEPTION("[bind] unknown family");
	}
	// バインド
	if(::bind(m_sock, &sockaddr.addr, addrlen) != 0) {
		RAISE_MYSOCKEXCEPTION("[bind] bind err=%d", ::WSAGetLastError());
	}
}

void MySock::CSocketBase::connect(const MySock::MySockAddr& sockaddr) {
	// check
	if(m_sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[connect] socket isn't created!!");
	}

	int addrlen = 0;
	if(sockaddr.addr.sa_family == AF_INET) {
		addrlen = sizeof(sockaddr.v4);
	} else if(sockaddr.addr.sa_family == AF_INET6) {
		addrlen = sizeof(sockaddr.v6);
	} else {
		RAISE_MYSOCKEXCEPTION("[connect] unknown family");
	}
	// 接続
	if(::connect(m_sock, &sockaddr.addr, addrlen) != 0) {
		int err = ::WSAGetLastError();
		if(err != WSAEWOULDBLOCK) {
			RAISE_MYSOCKEXCEPTION("[connect] connect err=%d", ::WSAGetLastError());
		}
	}
	// 接続先アドレスのキャッシュクリア
	m_peerSockAddr.addr.sa_family = AF_UNSPEC;
}

void MySock::CSocketBase::send(const MyLib::Data::BinaryData& data) {
	// check
	if(m_sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[send] socket isn't created!!");
	}

	// send
	int sendRet = ::send(m_sock, reinterpret_cast<const char*>(&data[0]), data.size(), 0);
	if(sendRet == SOCKET_ERROR) {
		RAISE_MYSOCKEXCEPTION("[send] sendto err=%d", ::WSAGetLastError());
	}
	if(sendRet != data.size()) {
		RAISE_MYSOCKEXCEPTION("[send] not equal datasize(%d) sendsize(%d)", data.size(), sendRet);
	}
}

void MySock::CSocketBase::setBlockingMode(bool isBlock) {
	// check
	if(m_sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[setBlockingMode] socket isn't created!!");
	}

	u_long arg = 0;	// blocking mode
	if(!isBlock) {
		arg = 1;	// non-blocking mode
	}
	if(::ioctlsocket(m_sock, FIONBIO, &arg) != 0) {
		RAISE_MYSOCKEXCEPTION("[setBlockingMode] ioctlsocket err=%d", ::WSAGetLastError());
	}
}

MySock::MySockAddr MySock::CSocketBase::getSockAddr() {
	// cache
	if(m_mySockAddr.addr.sa_family != AF_UNSPEC) {
		return m_mySockAddr;
	}

	// check
	if(m_sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[getSockAddr] socket isn't created!!");
	}

	// get
	int sockaddrlen = sizeof(m_mySockAddr.v4);
	if(m_family == AF_INET6) {
		sockaddrlen = sizeof(m_mySockAddr.v6);
	}
	int ret = ::getsockname(m_sock, &m_mySockAddr.addr, &sockaddrlen);
	if(ret != 0) {
		RAISE_MYSOCKEXCEPTION("[getSockAddr] getsockname err=%d", ::WSAGetLastError());
	}
	return m_mySockAddr;
}

MySock::MySockAddr MySock::CSocketBase::getPeerAddr() {
	// cache
	if(m_peerSockAddr.addr.sa_family != AF_UNSPEC) {
		return m_peerSockAddr;
	}

	// check
	if(m_sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[getPeerAddr] socket isn't created!!");
	}

	// get
	int sockaddrlen = sizeof(m_peerSockAddr.v4);
	if(m_family == AF_INET6) {
		sockaddrlen = sizeof(m_peerSockAddr.v6);
	}
	int ret = ::getpeername(m_sock, &m_peerSockAddr.addr, &sockaddrlen);
	if(ret != 0) {
		RAISE_MYSOCKEXCEPTION("[getPeerAddr] getsockname err=%d", ::WSAGetLastError());
	}
	return m_peerSockAddr;
}

bool MySock::CSocketBase::setRecvBuffSize(int size) {
	// check
	if(	(size < MIN_RECV_BUFFSIZE) ||
		(size > MAX_RECV_BUFFSIZE) ) {
		return false;
	}
	if(size == m_recvBufferSize) {
		return true;
	}

	// resize
	m_recvBufferSize = size;
	m_recvBuffer.resize(m_recvBufferSize, 0);

	return true;
}
