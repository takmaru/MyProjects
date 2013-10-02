#pragma once

#include <WinSock2.h>

#include <vector>

#include <MyLib/Data/BinaryData.h>

#include "MySockTypedef.h"
#include "SocketBase.h"

namespace MySock {

class CTCPSocket : public MySock::CSocketBase {
public:
	CTCPSocket();
	CTCPSocket(const CTCPSocket& obj);
	CTCPSocket(SOCKET sock, int family);
	~CTCPSocket();

public:
	void create(int family);
	void listen();
	CTCPSocket accept(MySock::MySockAddr* sockaddr = NULL);
};
typedef std::vector<CTCPSocket> TCPSocketList;

}
