#pragma once

#include <Windows.h>
#include <WinSock2.h>

#include <MyLib/Data/BinaryData.h>

#include "../MySockTypedef.h"

namespace MySock {

class CUDPServer {
public:
	explicit CUDPServer(unsigned int recvTimeout = 0);
	~CUDPServer();

public:
	void start(unsigned short port);
	void start(const char* service);
	void stop();

	MyLib::Data::BinaryData recv(unsigned int timeout = 0);

	void setRecvTimeout(unsigned int timeout);

private:
	SOCKET_LIST m_sockets;
	timeval m_recvTimeout;
};

}