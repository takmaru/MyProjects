#pragma once

#include <WinSock2.h>

#include <vector>

#include <MyLib/Data/BinaryData.h>

#include "SocketBase.h"
#include "MySockTypedef.h"

namespace MySock {

class CUDPSocket : public MySock::CSocketBase {
public:
	CUDPSocket();
	CUDPSocket(const CUDPSocket& obj);
	CUDPSocket(SOCKET sock, int family);
	virtual ~CUDPSocket();

public:
	void create(int family);
	void recv(MyLib::Data::BinaryData& data, MySock::MySockAddr* sockaddr, bool& isSendError);
	void sendTo(const MySock::MySockAddr& sockaddr, const MyLib::Data::BinaryData& data);
public:
	virtual int protocol() const {
		return IPPROTO_UDP;
	}
};

}
