#pragma once

#include <WinSock2.h>

#include <MyLib/Data/BinaryData.h>

#include "MySockTypedef.h"

namespace MySock {

class CUDPSocket {
public:
	CUDPSocket();
	explicit CUDPSocket(CUDPSocket& obj);
	~CUDPSocket();

	SOCKET release();

public:
	void create(int family);
	void close();

	void bind(const MySock::MySockAddr& sockaddr);
	void connect(const MySock::MySockAddr& sockaddr);

	void recv(MyLib::Data::BinaryData& data, MySock::MySockAddr* sockaddr = NULL);

	void sendTo(const MySock::MySockAddr& sockaddr, const MyLib::Data::BinaryData& data);
	void send(const MyLib::Data::BinaryData& data);

public:
	bool setRecvBuffSize(int size);

	SOCKET socket() const {
		return m_sock;
	};

private:
	SOCKET m_sock;
	int m_family;
	int m_recvBufferSize;
	MyLib::Data::BinaryData m_recvBuffer;
};

}
