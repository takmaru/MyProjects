#include "stdafx.h"
#include "UDPServer.h"

#include <WS2tcpip.h>

#include <MyLib/tstring/tstring.h>
#include "../MySockException.h"

namespace {
	const unsigned int DEFAULT_RECV_TIMEOUT = 100;
	const unsigned int DEFAULT_RECV_BUFFSIZE = 4 * 1024;
	const unsigned int MIN_RECV_BUFFSIZE = 256;
	const unsigned int MAX_RECV_BUFFSIZE = 4 * 1024 * 1024;
}

MySock::CUDPServer::CUDPServer(unsigned int recvTimeout/*= 0*/, int recvBuffSize/*= 0*/):
	m_sockets(), m_recvTimeout(),
	m_recvBufferSize(DEFAULT_RECV_BUFFSIZE), m_recvBuffer(DEFAULT_RECV_BUFFSIZE, 0), m_recvBufferQueue(),
	m_starterrors() {
	this->setRecvTimeout(recvTimeout);
	this->setRecvBuffSize(recvBuffSize);
}
MySock::CUDPServer::~CUDPServer() {
}

void MySock::CUDPServer::start(unsigned short port) {
	std::ostringstream oss;
	oss << port;
	this->start(oss.str().c_str());
}
void MySock::CUDPServer::start(const char* service) {

	if(m_sockets.size() > 0) {
		RAISE_MYSOCKEXCEPTION("start is starting");
	}

	// アドレス情報取得
	ADDRINFOA addrHints = {0};
	addrHints.ai_family = AF_UNSPEC;
	addrHints.ai_socktype = SOCK_DGRAM;
	addrHints.ai_protocol = IPPROTO_UDP;
	addrHints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
	PADDRINFOA addrResults = NULL;
	int ret = ::getaddrinfo(NULL, service, &addrHints, &addrResults);
	if(ret != 0) {
		RAISE_MYSOCKEXCEPTION("start getaddrinfo ret=%d err=%d", ret, ::WSAGetLastError());
	}

	// アドレス情報は複数の可能性があるので、ループしてソケット生成＆バインドを実施する
	m_starterrors.clear();
	SOCKET_LIST sockets;
	for(PADDRINFOA p = addrResults; p != NULL; p = p->ai_next) {
		// ソケット作成
		SOCKET sock = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if(sock == INVALID_SOCKET) {
			m_starterrors.push_back(udp_start_error(*p, ::WSAGetLastError(), 0));
			continue;
		}
		// バインド
		if(::bind(sock, p->ai_addr, p->ai_addrlen) != 0) {
			::closesocket(sock);
			m_starterrors.push_back(udp_start_error(*p, 0, ::WSAGetLastError()));
			continue;
		}
		// ソケットリストに追加
		sockets.push_back(sock);
	}
	::freeaddrinfo(addrResults);
	if(sockets.size() == 0) {
		RAISE_MYSOCKEXCEPTION("start socket create or bind err");
	}

	m_sockets.swap(sockets);
}

void MySock::CUDPServer::stop() {
	for(SOCKET_LIST::iterator it = m_sockets.begin(); it != m_sockets.end(); ++it) {
		if(::closesocket(*it) != 0) {
			RAISE_MYSOCKEXCEPTION("stop closesocket err=%d", ::WSAGetLastError());
		}
	}
	m_sockets.clear();
}

bool MySock::CUDPServer::recv(MyLib::Data::BinaryData& data, unsigned int timeout/*= 0*/) {
	// fd_set を構築
	fd_set reads = {0};
	fd_set excepts = {0};
	FD_ZERO(&reads);
	FD_ZERO(&excepts);
	for(SOCKET_LIST::iterator it = m_sockets.begin(); it != m_sockets.end(); ++it) {
		FD_SET(*it, &reads);
		FD_SET(*it, &excepts);
	}
	// タイムアウト値
	timeval selectTimeout = m_recvTimeout;
	if(timeout != 0) {
		selectTimeout.tv_sec = timeout / 1000;
		selectTimeout.tv_usec = timeout - (m_recvTimeout.tv_sec * 1000);
	}
	// select
	int ret = ::select(0, &reads, NULL, &excepts, &selectTimeout);
	if(ret == SOCKET_ERROR) {
		RAISE_MYSOCKEXCEPTION("recv select err=%d", ::WSAGetLastError());
	}

	bool isRecv = false;
	if(ret > 0) {
		// on event
		for(SOCKET_LIST::iterator it = m_sockets.begin(); it != m_sockets.end(); ++it) {
			SOCKET sock = *it;
			if(FD_ISSET(sock, &excepts)) {
				// error
			} else if(FD_ISSET(sock, &reads)) {
				// read
				MySockAddr mySockAddr = {0};
				PSOCKADDR sockaddrPtr = reinterpret_cast<PSOCKADDR>(&mySockAddr.v4);
				int sockaddrLength = sizeof(mySockAddr.v4);
				::recvfrom(sock, (char*)&m_recvBuffer[0], m_recvBuffer.size(), 0, sockaddrPtr, &sockaddrLength);
			}
		}
	}

	return isRecv;
}

void MySock::CUDPServer::setRecvTimeout(unsigned int timeout) {
	if(timeout == 0) {
		timeout = DEFAULT_RECV_TIMEOUT;
	}
	m_recvTimeout.tv_sec = timeout / 1000;
	m_recvTimeout.tv_usec = timeout - (m_recvTimeout.tv_sec * 1000);
}

void MySock::CUDPServer::setRecvBuffSize(int size) {
	if(	(size < MIN_RECV_BUFFSIZE) ||
		(size > MAX_RECV_BUFFSIZE) ||
		(size == m_recvBufferSize)	) {
		return;
	}
	m_recvBufferSize = size;
	m_recvBuffer.resize(m_recvBufferSize, 0);
}
