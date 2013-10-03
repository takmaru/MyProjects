#pragma once

#include <WinSock2.h>

#include <vector>

#include <MyLib/Data/BinaryData.h>

#include "MySockTypedef.h"

namespace MySock {

class CSocketBase {
public:
	CSocketBase();
	CSocketBase(const CSocketBase& obj);
	CSocketBase(SOCKET sock, int family);
	virtual ~CSocketBase();

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

protected:
	SOCKET m_sock;
	int m_family;
	MySock::MySockAddr m_mySockAddr;
	MySock::MySockAddr m_peerSockAddr;
	int m_recvBufferSize;
	MyLib::Data::BinaryData m_recvBuffer;
};
typedef std::vector<CSocketBase> SocketList;

}
