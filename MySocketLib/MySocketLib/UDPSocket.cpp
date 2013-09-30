#include "stdafx.h"
#include "UDPSocket.h"

#include <sstream>

#include "MySockException.h"

namespace {
	const unsigned int DEFAULT_RECV_BUFFSIZE = 4 * 1024;
	const unsigned int MIN_RECV_BUFFSIZE = 256;
	const unsigned int MAX_RECV_BUFFSIZE = 4 * 1024 * 1024;
}

MySock::CUDPSocket::CUDPSocket():
	m_sock(INVALID_SOCKET), m_family(AF_UNSPEC), m_mySockAddr(), m_peerSockAddr(),
	m_recvBufferSize(DEFAULT_RECV_BUFFSIZE), m_recvBuffer(m_recvBufferSize, 0) {
	m_mySockAddr.addr.sa_family = AF_UNSPEC;
	m_peerSockAddr.addr.sa_family = AF_UNSPEC;
}

MySock::CUDPSocket::CUDPSocket(const CUDPSocket& obj):
	m_sock(obj.m_sock), m_family(obj.m_family), m_mySockAddr(obj.m_mySockAddr), m_peerSockAddr(obj.m_peerSockAddr),
	m_recvBufferSize(obj.m_recvBufferSize), m_recvBuffer(m_recvBufferSize, 0) {
}

MySock::CUDPSocket::~CUDPSocket() {
}

void MySock::CUDPSocket::create(int family) {
	// check
	if(m_sock != INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[create] socket is created!!");
	}

	m_sock = ::socket(family, SOCK_DGRAM, IPPROTO_UDP);
	if(m_sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[create] socket err=%d", ::WSAGetLastError());
	}
	m_family = family;
}
void MySock::CUDPSocket::close() {
	if(m_sock != INVALID_SOCKET) {
		if(::closesocket(m_sock) != 0) {
			std::tcerr << _T("CUDPSocket::close closesocket err=") << ::WSAGetLastError() << std::endl;
		}
		m_sock = INVALID_SOCKET;
	}
}

void MySock::CUDPSocket::bind(const MySock::MySockAddr& sockaddr) {
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
	// ƒoƒCƒ“ƒh
	if(::bind(m_sock, &sockaddr.addr, addrlen) != 0) {
		RAISE_MYSOCKEXCEPTION("[bind] bind err=%d", ::WSAGetLastError());
	}
}

void MySock::CUDPSocket::connect(const MySock::MySockAddr& sockaddr) {
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
	// Ú‘±
	if(::connect(m_sock, &sockaddr.addr, addrlen) != 0) {
		RAISE_MYSOCKEXCEPTION("[connect] connect err=%d", ::WSAGetLastError());
	}
}

void MySock::CUDPSocket::recv(MyLib::Data::BinaryData& data, MySock::MySockAddr* sockaddr/*= NULL*/) {
	// check
	if(m_sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[recv] socket isn't created!!");
	}

	// recv
	MySockAddr mySockAddr = {0};
	int sockaddrlen = sizeof(mySockAddr.v4);
	if(m_family == AF_INET6) {
		sockaddrlen = sizeof(mySockAddr.v6);
	}
	int recvRet = ::recvfrom(m_sock, reinterpret_cast<char*>(&m_recvBuffer[0]), m_recvBuffer.size(), 0, reinterpret_cast<PSOCKADDR>(&mySockAddr.addr), &sockaddrlen);
	if(recvRet == SOCKET_ERROR) {
		RAISE_MYSOCKEXCEPTION("recv recvfrom err=%d", ::WSAGetLastError());
	}

	// result set
	data.resize(recvRet);
	data.assign(m_recvBuffer.begin(), m_recvBuffer.begin() + recvRet);
	if(sockaddr != NULL) {
		*sockaddr = mySockAddr;
	}
}

void MySock::CUDPSocket::sendTo(const MySock::MySockAddr& sockaddr, const MyLib::Data::BinaryData& data) {
	// check
	if(m_sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[sendTo] socket isn't created!!");
	}

	// send
	int sockaddrlen = sizeof(sockaddr.v4);
	if(sockaddr.addr.sa_family == AF_INET6) {
		sockaddrlen = sizeof(sockaddr.v6);
	}
	int sendRet = ::sendto(m_sock, reinterpret_cast<const char*>(&data[0]), data.size(), 0, &sockaddr.addr, sockaddrlen);
	if(sendRet == SOCKET_ERROR) {
		RAISE_MYSOCKEXCEPTION("[sendTo] sendto err=%d", ::WSAGetLastError());
	}
	if(sendRet != data.size()) {
		RAISE_MYSOCKEXCEPTION("[sendTo] not equal datasize(%d) sendsize(%d)", data.size(), sendRet);
	}
}
void MySock::CUDPSocket::send(const MyLib::Data::BinaryData& data) {
	// check
	if(m_sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[send] socket isn't created!!");
	}

	// send
	int sendRet = ::send(m_sock, reinterpret_cast<const char*>(&data[0]), data.size(), 0);
	if(sendRet == SOCKET_ERROR) {
		RAISE_MYSOCKEXCEPTION("[sendTo] sendto err=%d", ::WSAGetLastError());
	}
	if(sendRet != data.size()) {
		RAISE_MYSOCKEXCEPTION("[sendTo] not equal datasize(%d) sendsize(%d)", data.size(), sendRet);
	}
}

MySock::MySockAddr MySock::CUDPSocket::getSockAddr() {
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

MySock::MySockAddr MySock::CUDPSocket::getPeerAddr() {
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

bool MySock::CUDPSocket::setRecvBuffSize(int size) {
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
