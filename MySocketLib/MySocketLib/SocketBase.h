#pragma once

#include <WinSock2.h>

#include <set>
#include <list>

#include <MyLib/Data/BinaryData.h>

#include "MySockTypedef.h"

namespace MySock {

enum SocketState {
	kSockState_Invalid = 0,
	kSockState_Created,
	kSockState_Binded,
	kSockState_WaitConnect,
	kSockState_Connecting,
	kSockState_Listening,
	kSockState_GracefulClosing,
	kSockState_Closed
};
typedef unsigned int SocketIOState;
const SocketIOState kSocketIOState_Readable			= 0x00000001;
const SocketIOState kSocketIOState_Acceptable		= 0x00000001;
const SocketIOState kSocketIOState_Writeable		= 0x00000002;
const SocketIOState kSocketIOState_RecvOutOfBand	= 0x00000004;
const SocketIOState kSocketIOState_RecvFin			= 0x00000010;
const SocketIOState kSocketIOState_SendFin			= 0x00000020;

class CSocketBase {
public:
	CSocketBase();
	CSocketBase(const MySock::CSocketBase& obj);
	CSocketBase(SOCKET sock, int family);
	virtual ~CSocketBase();

public:
	bool operator<(const MySock::CSocketBase& obj);

protected:
	void create_socket(int family, int type, int protocol);

public:
	void close();
	void shutdown(int how);

	void bind(const MySock::MySockAddr& sockaddr);
	void connect(const MySock::MySockAddr& sockaddr);

	void send(const MyLib::Data::BinaryData& data);

	void setBlockingMode(bool isBlock);

public:
	MySock::MySockAddr getSockAddr();
	MySock::MySockAddr getPeerAddr();

public:
	bool setRecvBuffSize(int size);

	SOCKET socket() const {
		return m_sock;
	};
	virtual int protocol() const = 0;

	void setState(SocketState State) {
		m_state = State;
	}
	SocketState state() const {
		return m_state;
	}

	void setIOState(SocketIOState state) {
		m_ioState |= state;
	}
	void resetIOState(SocketIOState state) {
		m_ioState &= ~state;
	}
	bool isIOState(SocketIOState state) const {
		return ((m_ioState & state) != 0);
	}
	SocketIOState ioState() const {
		return m_ioState;
	}

	bool isBlocking() const {
		return m_isBlocking;
	}

protected:
	SOCKET m_sock;
	int m_family;
	MySock::MySockAddr m_mySockAddr;
	MySock::MySockAddr m_peerSockAddr;
	int m_recvBufferSize;
	MyLib::Data::BinaryData m_recvBuffer;

	SocketState m_state;
	SocketIOState m_ioState;
	bool m_isBlocking;
};

class compareSocketBase {
public:
	bool operator()(CSocketBase* obj1, CSocketBase* obj2) const {
		return (obj1->socket() < obj2->socket());
	}
};
typedef std::set<CSocketBase*, compareSocketBase> SocketSet;
typedef std::list<CSocketBase*> SocketList;

}
