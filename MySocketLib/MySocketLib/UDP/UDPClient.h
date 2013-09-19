#pragma once

#include <MyLib/Data/BinaryData.h>

#include "../MySockTypedef.h"

namespace MySock {

class CUDPClient {
public:
	CUDPClient();
	~CUDPClient();

public:
	bool sendTo(const char* host, unsigned short port, const MyLib::Data::BinaryData& data);
	bool sendTo(const char* host, const char* service, const MyLib::Data::BinaryData& data);

	void connect(const char* host, unsigned short port);
	void connect(const char* host, const char* service);
	void disconnect();
	bool send(const MyLib::Data::BinaryData& data);

	MYSOCKERRORS sock_errors() const {
		return m_sockerrors;
	}
private:
	SocketAddr m_connectSocket;
	PADDRINFOA m_addrInfos;
	MYSOCKERRORS m_sockerrors;
};

}
