#pragma once

#include <WinSock2.h>

#include <set>

#include <MyLib/Data/BinaryData.h>

#include "MySockTypedef.h"

namespace MySock {

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

	void setIsConnected(bool v) {
		m_isConnected = v;
	};
	void setIsWriteable(bool v) {
		m_isWriteable = v;
	};
	void setIsReadable(bool v) {
		m_isReadable = v;
	};
	bool isConnected() const {
		return m_isConnected;
	};
	bool isWriteable() const {
		return m_isWriteable;
	};
	bool isReadable() const {
		return m_isReadable;
	};
protected:
	SOCKET m_sock;
	int m_family;
	MySock::MySockAddr m_mySockAddr;
	MySock::MySockAddr m_peerSockAddr;
	int m_recvBufferSize;
	MyLib::Data::BinaryData m_recvBuffer;

	bool m_isConnected;
	bool m_isWriteable;
	bool m_isReadable;
};

class compareSocketBase {
public:
	bool operator()(CSocketBase* obj1, CSocketBase* obj2) const {
		return (obj1->socket() < obj2->socket());
	}
};
typedef std::set<CSocketBase*, compareSocketBase> SocketSet;

}
