#include "stdafx.h"
#include "UDPClient.h"

#include <WS2tcpip.h>

#include <sstream>

#include "../MySockException.h"

MySock::CUDPClient::CUDPClient() : m_connectSocket(INVALID_SOCKET, NULL), m_addrInfos(NULL), m_sockerrors() {
}
MySock::CUDPClient::~CUDPClient() {
}

bool MySock::CUDPClient::sendTo(const char* host, unsigned short port, const MyLib::Data::BinaryData& data) {
	std::ostringstream oss;
	oss << port;
	return this->sendTo(host, oss.str().c_str(), data);
}

bool MySock::CUDPClient::sendTo(const char* host, const char* service, const MyLib::Data::BinaryData& data) {

	// アドレス情報取得
	ADDRINFOA addrHints = {0};
	addrHints.ai_family = AF_UNSPEC;
	addrHints.ai_socktype = SOCK_DGRAM;
	addrHints.ai_protocol = IPPROTO_UDP;
	addrHints.ai_flags = 0;
	PADDRINFOA addrResults = NULL;
	int ret = ::getaddrinfo(host, service, &addrHints, &addrResults);
	if(ret != 0) {
		RAISE_MYSOCKEXCEPTION("send getaddrinfo ret=%d err=%d", ret, ::WSAGetLastError());
	}

	bool result = false;
	m_sockerrors.clear();
	for(PADDRINFOA p = addrResults; p != NULL; p = p->ai_next) {
		// ソケット作成
		SOCKET sock = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if(sock == INVALID_SOCKET) {
			m_sockerrors.push_back(mysock_error_info(MySock::MySockErrorType::send, ::WSAGetLastError(), *p));
			continue;
		}
		// 送信
		int sendRet = ::sendto(sock, reinterpret_cast<const char*>(&data[0]), data.size(), 0, p->ai_addr, p->ai_addrlen);
		::closesocket(sock);
		if(sendRet == data.size()) {
			result = true;
			break;
		} else {
			m_sockerrors.push_back(mysock_error_info(MySock::MySockErrorType::less_sendlen, 0, *p));
		}
	}

	::freeaddrinfo(addrResults);

	return result;
}

void MySock::CUDPClient::connect(const char* host, unsigned short port) {
	std::ostringstream oss;
	oss << port;
	this->connect(host, oss.str().c_str());
}
void MySock::CUDPClient::connect(const char* host, const char* service) {

	if(m_connectSocket.sock != INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("connect is connecting");
	}

	// アドレス情報取得
	ADDRINFOA addrHints = {0};
	addrHints.ai_family = AF_UNSPEC;
	addrHints.ai_socktype = SOCK_DGRAM;
	addrHints.ai_protocol = IPPROTO_UDP;
	addrHints.ai_flags = 0;
	PADDRINFOA addrResults = NULL;
	int ret = ::getaddrinfo(host, NULL, &addrHints, &addrResults);
	if(ret != 0) {
		RAISE_MYSOCKEXCEPTION("connect getaddrinfo ret=%d err=%d", ret, ::WSAGetLastError());
	}

	bool result = false;
	m_sockerrors.clear();
	SocketAddr socketAddr(INVALID_SOCKET, NULL);
	for(PADDRINFOA p = addrResults; p != NULL; p = p->ai_next) {
		// ソケット作成
		SOCKET sock = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if(sock == INVALID_SOCKET) {
			m_sockerrors.push_back(mysock_error_info(MySock::MySockErrorType::send, ::WSAGetLastError(), *p));
			continue;
		}
		// 接続
		MySock::MySockAddr mySockAddr = {0};
		PSOCKADDR sockaddr = reinterpret_cast<PSOCKADDR>(&mySockAddr);
		int sockaddrLength = sizeof(mySockAddr.v4);
		if(p->ai_family == AF_INET6) {
			sockaddrLength = sizeof(mySockAddr.v6);
		}
		if(::connect(sock, p->ai_addr, p->ai_addrlen) == 0) {
			socketAddr.sock = sock;
			socketAddr.addr = p;
			break;
		} else {
			m_sockerrors.push_back(mysock_error_info(MySock::MySockErrorType::connect, ::WSAGetLastError(), *p));
		}
		::closesocket(sock);
	}

	if(socketAddr.sock == INVALID_SOCKET) {
		::freeaddrinfo(addrResults);
		RAISE_MYSOCKEXCEPTION("connect socket create or connect err");
	}

	m_connectSocket = socketAddr;
	m_addrInfos = addrResults;
}

void MySock::CUDPClient::disconnect() {

	if(m_connectSocket.sock == INVALID_SOCKET) {
		RAISE_MYSOCKEXCEPTION("disconnect isn't connecting");
	}

	if(::closesocket(m_connectSocket.sock) != 0) {
		RAISE_MYSOCKEXCEPTION("disconnect closesocket err=%d", ::WSAGetLastError());
	}
	m_connectSocket.sock = INVALID_SOCKET;
	m_connectSocket.addr = NULL;
	::freeaddrinfo(m_addrInfos);
	m_addrInfos = NULL;
}

bool MySock::CUDPClient::send(const MyLib::Data::BinaryData& data) {
	return false;
}
