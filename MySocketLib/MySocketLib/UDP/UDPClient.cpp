#include "stdafx.h"
#include "UDPClient.h"

#include <WS2tcpip.h>

#include <sstream>

#include "../MySockException.h"

MySock::CUDPClient::CUDPClient() : m_senderrors() {
}
MySock::CUDPClient::~CUDPClient() {
}

bool MySock::CUDPClient::send(const char* host, unsigned short port, const MyLib::Data::BinaryData& data) {
	std::ostringstream oss;
	oss << port;
	return this->send(host, oss.str().c_str(), data);
}

bool MySock::CUDPClient::send(const char* host, const char* service, const MyLib::Data::BinaryData& data) {

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
	m_senderrors.clear();
	for(PADDRINFOA p = addrResults; p != NULL; p = p->ai_next) {
		// ソケット作成
		SOCKET sock = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if(sock == INVALID_SOCKET) {
			m_senderrors.push_back(mysock_error_info(MySock::MySockErrorType::send, ::WSAGetLastError(), *p));
			continue;
		}
		// 送信
		int sendRet = ::sendto(sock, reinterpret_cast<const char*>(&data[0]), data.size(), 0, p->ai_addr, p->ai_addrlen);
		if(sendRet == data.size()) {
			result = true;
			break;
		} else {
			m_senderrors.push_back(mysock_error_info(MySock::MySockErrorType::less_sendlen, 0, *p));
		}
	}

	::freeaddrinfo(addrResults);

	return result;
}
