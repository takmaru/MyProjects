#include "stdafx.h"
#include "UDPServer.h"

#include <WS2tcpip.h>

#include "../MySockException.h"

namespace {
	const unsigned int DEFAULT_RECV_TIMEOUT = 100;
}

MySock::CUDPServer::CUDPServer(unsigned int recvTimeout/*= 0*/) : m_sockets(), m_recvTimeout()  {
	this->setRecvTimeout(recvTimeout);
}
MySock::CUDPServer::~CUDPServer() {
}

void MySock::CUDPServer::start(unsigned short port) {
	std::ostringstream oss;
	oss << port;
	this->start(oss.str().c_str());
}
void MySock::CUDPServer::start(const char* service) {

	// アドレス情報取得
	MyLib::Data::BinaryData service_work(service, service + strlen(service));
	ADDRINFOA addrHints = {0};
	addrHints.ai_family = AF_UNSPEC;
	addrHints.ai_socktype = SOCK_DGRAM;
	addrHints.ai_protocol = IPPROTO_UDP;
	addrHints.ai_flags = AI_PASSIVE;
	PADDRINFOA addrResult = NULL;
	int ret = ::getaddrinfo(NULL, (PCSTR)&service_work[0], &addrHints, &addrResult);
	if(ret != 0) {
		RAISE_MYSOCKEXCEPTION("start getaddrinfo ret=%d err=%d", ret, ::WSAGetLastError());
	}

	// アドレス情報は複数の可能性があるので、ループしてソケット生成＆バインドを実施する
	for(PADDRINFOA p = addrResult; p != NULL; p = p->ai_next) {
		// ソケット作成
		SOCKET sock = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if(sock == INVALID_SOCKET) {
			RAISE_MYSOCKEXCEPTION("start create socket err=%d", ::WSAGetLastError());
		}
		// バインド
		if(::bind(sock, p->ai_addr, p->ai_addrlen) != 0) {
			RAISE_MYSOCKEXCEPTION("start bind err=%d", ::WSAGetLastError());
		}
		// ソケットリストに追加
		m_sockets.push_back(sock);
	}
	::freeaddrinfo(addrResult);
}

void MySock::CUDPServer::stop() {
}

MyLib::Data::BinaryData MySock::CUDPServer::recv(unsigned int timeout/*= 0*/) {
	return MyLib::Data::BinaryData();
}

void MySock::CUDPServer::setRecvTimeout(unsigned int timeout) {
	if(timeout == 0) {
		timeout = DEFAULT_RECV_TIMEOUT;
	}
	m_recvTimeout.tv_sec = timeout / 1000;
	m_recvTimeout.tv_usec = timeout - (m_recvTimeout.tv_sec * 1000);
}