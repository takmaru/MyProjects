#include "stdafx.h"
#include "UDPServer.h"

#include <WS2tcpip.h>

#include <MyLib/tstring/tstring.h>
#include "../MySockException.h"

namespace {
	const unsigned int DEFAULT_RECV_TIMEOUT = 100;
}

MySock::CUDPServer::CUDPServer(unsigned int recvTimeout/*= 0*/) : m_sockets(), m_recvTimeout(), m_starterrors() {
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

	if(m_sockets.size() > 0) {
		RAISE_MYSOCKEXCEPTION("start is starting");
	}

	// �A�h���X���擾
	MyLib::Data::BinaryData service_work(service, service + strlen(service) + 1);
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

	// �A�h���X���͕����̉\��������̂ŁA���[�v���ă\�P�b�g�������o�C���h�����{����
	m_starterrors.clear();
	SOCKET_LIST sockets;
	for(PADDRINFOA p = addrResult; p != NULL; p = p->ai_next) {
		// �\�P�b�g�쐬
		SOCKET sock = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if(sock == INVALID_SOCKET) {
			m_starterrors.push_back(udp_start_error(*p, ::WSAGetLastError(), 0));
			continue;
		}
		// �o�C���h
		if(::bind(sock, p->ai_addr, p->ai_addrlen) != 0) {
			::closesocket(sock);
			m_starterrors.push_back(udp_start_error(*p, 0, ::WSAGetLastError()));
			continue;
		}
		// �\�P�b�g���X�g�ɒǉ�
		sockets.push_back(sock);
	}
	::freeaddrinfo(addrResult);
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
