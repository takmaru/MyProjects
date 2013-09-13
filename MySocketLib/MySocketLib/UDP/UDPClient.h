#pragma once

#include <MyLib/Data/BinaryData.h>

#include "../MySockTypedef.h"

namespace MySock {

class CUDPClient {
public:
	CUDPClient();
	~CUDPClient();

public:
	bool send(const char* host, unsigned short port, const MyLib::Data::BinaryData& data);
	bool send(const char* host, const char* service, const MyLib::Data::BinaryData& data);
	MYSOCKERRORS send_errors() const {
		return m_senderrors;
	}

private:
	MYSOCKERRORS m_senderrors;
};

}
