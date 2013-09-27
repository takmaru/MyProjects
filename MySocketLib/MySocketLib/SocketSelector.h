#pragma once

#include <WinSock2.h>

#include <list>
#include <map>

#include "MySockTypedef.h"

namespace MySock {

typedef unsigned int SelectFlg;
const SelectFlg SelectWrite	= 0x1;
const SelectFlg SelectRead	= 0x2;
const SelectFlg SelectError	= 0x4;
typedef std::map<SelectFlg, SOCKET_LIST> SelectResults;

class CSocketSelector {
private:
	typedef std::map<SOCKET, SelectFlg> SelectSockets;
	typedef std::pair<SOCKET, SelectFlg> SelectSocketsPair;
/*
	struct _SelectSocket {
	public:
		_SelectSocket() : sock(INVALID_SOCKET), selectFlg(0) {}
		_SelectSocket(SOCKET s, unsigned int flg):
			sock(s), selectFlg(flg) {}
		~_SelectSocket() {}
	public:
		SOCKET sock;
		unsigned int selectFlg;
	} typedef SelectSocket;
*/

public:
	CSocketSelector();
	explicit CSocketSelector(unsigned int timeout);
	~CSocketSelector();

public:
	SelectResults select();
	SelectResults select(unsigned int timeout);

	void addSocket(SOCKET sock, unsigned int flg);
	void removeSocket(SOCKET sock, unsigned int flg);
	void clearSockets();

public:
	void setTimeout(unsigned int timeout);

private:
	SelectSockets m_sockets;
	unsigned int m_timeout;
};

}
