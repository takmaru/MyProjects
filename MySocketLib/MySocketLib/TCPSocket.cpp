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
	// 状態遷移：リッスン中
	m_state = kSockState_Listening;
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
	// accept通知Off
	this->resetIOState(kSocketIOState_Acceptable);

	// 接続ソケット作成 状態遷移：接続中
	MySock::CTCPSocket acceptSocket(sock, m_family);
	acceptSocket.setState(kSockState_Connecting);
	return acceptSocket;
}

MyLib::Data::BinaryData MySock::CTCPSocket::recv() {
	// check
	if(m_sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("[recv] socket isn't created!!");
	}

	MyLib::Data::BinaryData result;
	// recv
	int recvRet = ::recv(m_sock, reinterpret_cast<char*>(&m_recvBuffer[0]), m_recvBuffer.size(), 0);
	if(recvRet == SOCKET_ERROR) {
		RAISE_MYSOCKEXCEPTION("[recv] recv err=%d", ::WSAGetLastError());
	}

	// result set
	if(recvRet > 0) {
		// データ受信
		result.resize(recvRet);
		result.assign(m_recvBuffer.begin(), m_recvBuffer.begin() + recvRet);
		// Read通知Off
		this->resetIOState(kSocketIOState_Readable);
	} else if(recvRet == 0) {
		// 状態遷移：クローズ処理中
		m_state = kSockState_GracefulClosing;
		// Fin受信
		this->setIOState(kSocketIOState_RecvFin);
	}

	return result;
}
