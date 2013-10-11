#include "stdafx.h"
#include "SocketSelector.h"

#include "MySockException.h"

namespace {
	const unsigned int DEFAULT_RECV_TIMEOUT = 100;
}

MySock::CSocketSelector::CSocketSelector() : m_sockets(), m_timeout(DEFAULT_RECV_TIMEOUT) {
}
MySock::CSocketSelector::CSocketSelector(unsigned int timeout) : m_sockets(), m_timeout(timeout) {
}
MySock::CSocketSelector::~CSocketSelector() {
}

MySock::SelectResults MySock::CSocketSelector::select() {
	return this->select(m_timeout);
}
MySock::SelectResults MySock::CSocketSelector::select(unsigned int timeout) {
	// timeout確定
	timeval selectTimeout = {0};
	selectTimeout.tv_sec = timeout / 1000;
	selectTimeout.tv_usec = timeout - (selectTimeout.tv_sec * 1000);

	// fd_set を構築
	fd_set reads = {0};
	fd_set writes = {0};
	fd_set excepts = {0};
	FD_ZERO(&reads);
	FD_ZERO(&writes);
	FD_ZERO(&excepts);
	for(SelectSockets::iterator it = m_sockets.begin(); it != m_sockets.end(); ++it) {
		SOCKET sock = it->first;
		SelectFlg flg = it->second;
		if((flg & kSelectRead) != 0) {
			FD_SET(sock, &reads);
		}
		if((flg & kSelectWrite) != 0) {
			FD_SET(sock, &writes);
		}
		if((flg & kSelectExcept) != 0) {
			FD_SET(sock, &excepts);
		}
	}

	// select
	int selectRet = ::select(0, &reads, &writes, &excepts, ((timeout == INFINITE) ? NULL : &selectTimeout));
	if(selectRet == SOCKET_ERROR) {
		RAISE_MYSOCKEXCEPTION("[select] select err=%d", ::WSAGetLastError());
	}

	// 結果をセット
	MySock::SelectResults result;
	result.insert(SelectResultsPair(kSelectRead, MySock::SOCKET_LIST()));
	result.insert(SelectResultsPair(kSelectWrite, MySock::SOCKET_LIST()));
	result.insert(SelectResultsPair(kSelectExcept, MySock::SOCKET_LIST()));
	for(SelectSockets::iterator it = m_sockets.begin(); it != m_sockets.end(); ++it) {
		SOCKET sock = it->first;
		if(FD_ISSET(sock, &reads)) {
			result[kSelectRead].push_back(sock);
		}
		if(FD_ISSET(sock, &writes)) {
			result[kSelectWrite].push_back(sock);
		}
		if(FD_ISSET(sock, &excepts)) {
			result[kSelectExcept].push_back(sock);
		}
	}

	return result;
}

void MySock::CSocketSelector::addSocket(SOCKET sock, SelectFlg flg) {
	SelectSockets::iterator find = m_sockets.find(sock);
	if(find != m_sockets.end()) {
		find->second = flg;
	} else {
		m_sockets.insert(SelectSocketsPair(sock, flg));
	}
}
void MySock::CSocketSelector::removeSocket(SOCKET sock) {
	m_sockets.erase(sock);
}
void MySock::CSocketSelector::clearSockets() {
	m_sockets.clear();
}

void MySock::CSocketSelector::setTimeout(unsigned int timeout) {
	m_timeout = timeout;
}
