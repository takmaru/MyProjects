#pragma once

#include <WinSock2.h>

#include <vector>
#include <set>

#include <MyLib/Data/BinaryData.h>

#include "MySockTypedef.h"
#include "SocketBase.h"

namespace MySock {

class CTCPSocket : public MySock::CSocketBase {
public:
	CTCPSocket();
	CTCPSocket(const CTCPSocket& obj);
	CTCPSocket(SOCKET sock, int family);
	virtual ~CTCPSocket();

public:
	void create(int family);
	void listen();
	CTCPSocket accept(MySock::MySockAddr* sockaddr = NULL);
	MyLib::Data::BinaryData recv();
};
typedef std::vector<CTCPSocket> TCPSocketList;

}
