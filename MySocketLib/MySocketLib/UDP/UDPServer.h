#pragma once

#include <Windows.h>
#include <WinSock2.h>

#include <vector>
#include <queue>

#include <MyLib/Data/BinaryData.h>

#include "../MySockTypedef.h"

namespace MySock {

class CUDPServer {
private:
	struct _RecvDataAddr {
	public:
		_RecvDataAddr(const MyLib::Data::BinaryData& d, const MySockAddr& a):
			data(d), addr(a) {
		}
	public:
		MyLib::Data::BinaryData data;
		MySockAddr addr;
	} typedef RecvDataAddr;
	typedef std::queue<RecvDataAddr> RecvDataQueue;

public:
	explicit CUDPServer(unsigned int recvTimeout = 0, int recvBuffSize = 0);
	~CUDPServer();

public:
	void start(unsigned short port);
	void start(const char* service);
	UDP_STARTERRORS start_errors() const {
		return m_starterrors;
	}
	void stop();

	bool recv(MyLib::Data::BinaryData& data, MySockAddr* sockaddr = NULL, unsigned int timeout = 0);

	void setFamily(int family);
	void setRecvTimeout(unsigned int timeout);
	void setRecvBuffSize(int size);

private:
	SocketAddrList m_sockAddrs;
	PADDRINFOA m_addrInfos;
	int m_family;
	timeval m_recvTimeout;
	int m_recvBufferSize;
	MyLib::Data::BinaryData m_recvBuffer;
	RecvDataQueue m_recvDatasQueue;
	UDP_STARTERRORS m_starterrors;
};

}