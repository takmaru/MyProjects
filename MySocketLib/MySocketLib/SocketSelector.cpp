#include "stdafx.h"
#include "SocketSelector.h"

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
	timeval selectTimeout = {0};
	selectTimeout.tv_sec = timeout / 1000;
	selectTimeout.tv_usec = timeout - (selectTimeout.tv_sec * 1000);

	// TODO

	return MySock::SelectResults();
}

void MySock::CSocketSelector::addSocket(SOCKET sock, unsigned int flg) {
	SelectSockets::iterator find = m_sockets.find(sock);
	if(find != m_sockets.end()) {
		find->second = flg;
	} else {
		m_sockets.insert(SelectSocketsPair(sock, flg));
	}
}
void MySock::CSocketSelector::removeSocket(SOCKET sock, unsigned int flg) {
	m_sockets.erase(sock);
}
void MySock::CSocketSelector::clearSockets() {
	m_sockets.clear();
}

void MySock::CSocketSelector::setTimeout(unsigned int timeout) {
	m_timeout = timeout;
}
