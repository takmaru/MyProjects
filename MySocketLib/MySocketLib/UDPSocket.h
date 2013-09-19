#pragma once

#include <WinSock2.h>

#include <MyLib/Data/BinaryData.h>

#include "MySockTypedef.h"

namespace MySock {

class CUDPSocket {
public:
	CUDPSocket();
//	explicit CUDPSocket(SOCKET sock);
	~CUDPSocket();

public:
	void create(int family);
	void close();

	void bind(const MySock::MySockAddr& sockaddr);

	void recv(MyLib::Data::BinaryData& data, MySock::MySockAddr* sockaddr = NULL);

	void connect();

	bool sendTo(const char* host, unsigned short port, const MyLib::Data::BinaryData& data);
	bool sendTo(const char* host, const char* service, const MyLib::Data::BinaryData& data);
	bool send(const MyLib::Data::BinaryData& data);

public:
//	SOCKET release();

	bool setRecvBuffSize(int size);

	operator SOCKET() {
		return m_sock;
	}

private:
	SOCKET m_sock;
	int m_family;
	int m_recvBufferSize;
	MyLib::Data::BinaryData m_recvBuffer;
};

}
