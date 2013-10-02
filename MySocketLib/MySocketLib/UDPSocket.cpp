#include "stdafx.h"
#include "UDPSocket.h"

#include <sstream>

#include "MySockException.h"

MySock::CUDPSocket::CUDPSocket() : CSocketBase() {
}
MySock::CUDPSocket::CUDPSocket(const CUDPSocket& obj) : CSocketBase(obj) {
}
MySock::CUDPSocket::CUDPSocket(SOCKET sock, int family) : CSocketBase(sock, family) {
}
MySock::CUDPSocket::~CUDPSocket() {
}

void MySock::CUDPSocket::create(int family) {
	CSocketBase::create_socket(family, SOCK_DGRAM, IPPROTO_UDP);
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
	int recvRet = ::recvfrom(m_sock, reinterpret_cast<char*>(&m_recvBuffer[0]), m_recvBuffer.size(), 0, &mySockAddr.addr, &sockaddrlen);
	if(recvRet == SOCKET_ERROR) {
		RAISE_MYSOCKEXCEPTION("[recv] recvfrom err=%d", ::WSAGetLastError());
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
