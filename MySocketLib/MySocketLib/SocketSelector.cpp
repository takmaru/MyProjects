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
	// timeout�m��
	timeval selectTimeout = {0};
	selectTimeout.tv_sec = timeout / 1000;
	selectTimeout.tv_usec = timeout - (selectTimeout.tv_sec * 1000);

	// fd_set ���\�z
	fd_set reads = {0};
	fd_set writes = {0};
	fd_set excepts = {0};
	FD_ZERO(&reads);
	FD_ZERO(&writes);
	FD_ZERO(&excepts);
	for(MySock::SocketSet::iterator it = m_sockets.begin(); it != m_sockets.end(); ++it) {
		CSocketBase* sock = (*it);
		SOCKET nativeSock = (*it)->socket();
		if(sock->protocol() == IPPROTO_UDP) {
			// UDP�\�P�b�g
			if(	(sock->state() == kSockState_Binded) ||
				(sock->state() == kSockState_Connecting)	) {
				// �o�C���h���A�ڑ���
				if(!sock->isIOState(kSocketIOState_Readable)) {
					// Read�ʒmOFF
					FD_SET(nativeSock, &reads);
				}
			}
		} else if(sock->protocol() == IPPROTO_TCP) {
			// TCP�\�P�b�g
			if(sock->state() == kSockState_Listening) {
				// ���b�X����
				if(!sock->isIOState(kSocketIOState_Acceptable)) {
					// Accept�ʒmOFF
					FD_SET(nativeSock, &reads);
				}
			} else if(sock->state() == kSockState_WaitConnect) {
				// �ڑ��҂�
				FD_SET(nativeSock, &writes);
				FD_SET(nativeSock, &excepts);
			} else if(sock->state() == kSockState_Connecting) {
				// �ڑ���
				if(!sock->isIOState(kSocketIOState_Readable)) {
					// Read�ʒmOFF
					FD_SET(nativeSock, &reads);
				}
				if(!sock->isIOState(kSocketIOState_Writeable)) {
					// Write�ʒmOFF
					FD_SET(nativeSock, &writes);
				}
			} else if(sock->state() == kSockState_GracefulClosing) {
				// �N���[�Y������
				if(	!sock->isIOState(kSocketIOState_SendFin) &&
					!sock->isIOState(kSocketIOState_Writeable)	) {
					// Fin�����M�AWrite�ʒmOFF
					FD_SET(nativeSock, &writes);
				}
			}
		} else {
			RAISE_MYSOCKEXCEPTION("[select] invalid Socket(%d)", sock);
		}
	}

	if(	(reads.fd_count == 0) &&
		(writes.fd_count == 0) &&
		(excepts.fd_count == 0)	) {
		return MySock::SelectResults();
	}

	// select
	int selectRet = ::select(0, &reads, &writes, &excepts, ((timeout == INFINITE) ? NULL : &selectTimeout));
	if(selectRet == SOCKET_ERROR) {
		RAISE_MYSOCKEXCEPTION("[select] select err=%d", ::WSAGetLastError());
	}

	// ���ʂ��Z�b�g
	MySock::SelectResults result;
	for(MySock::SocketSet::iterator it = m_sockets.begin(); it != m_sockets.end(); ++it) {
		CSocketBase* sock = (*it);
		SOCKET nativeSock = (*it)->socket();
		if(FD_ISSET(nativeSock, &reads)) {
			// Read����ON
			if(	(sock->protocol() == IPPROTO_TCP) &&
				(sock->state() == kSockState_Listening)	) {
				// TCP�\�P�b�g�����b�X����
				result[kResultAcceptable].insert(sock);
				// Accept�ʒmON
				sock->setIOState(MySock::kSocketIOState_Acceptable);
			} else {
				// ���̑�
				result[kResultReadable].insert(sock);
				// Read�ʒmON
				sock->setIOState(MySock::kSocketIOState_Readable);
			}
		}
		if(FD_ISSET(nativeSock, &writes)) {
			// Write����ON
			if(sock->state() == kSockState_WaitConnect) {
				result[kResultConnectSuccess].insert(sock);
				// ��ԑJ�ځF�ڑ���
				sock->setState(kSockState_Connecting);
			}
			result[kResultWriteable].insert(sock);
			// Write�ʒmON
			sock->setIOState(MySock::kSocketIOState_Writeable);
		}
		if(FD_ISSET(nativeSock, &excepts)) {
			// Except ON
			if(sock->state() == kSockState_WaitConnect) {
				result[kResultConnectFailed].insert(sock);
				// ��ԑJ�ځF�\�P�b�g�쐬�ς�
				sock->setState(kSockState_Created);
			} else {
				// TODO: sock->setState(kSockState_?);
				result[kResultExcept].insert(sock);
			}
		}
	}

	return result;
}

void MySock::CSocketSelector::addSocket(CSocketBase* sock) {
	MySock::SocketSet::iterator find = m_sockets.find(sock);
	if(find == m_sockets.end()) {
		m_sockets.insert(sock);
	}
}
void MySock::CSocketSelector::removeSocket(CSocketBase* sock) {
	m_sockets.erase(sock);
}
void MySock::CSocketSelector::clearSockets() {
	m_sockets.clear();
}

void MySock::CSocketSelector::setTimeout(unsigned int timeout) {
	m_timeout = timeout;
}
