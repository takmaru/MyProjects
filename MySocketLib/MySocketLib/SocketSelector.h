#pragma once

#include <WinSock2.h>

#include <list>
#include <map>

#include "MySockTypedef.h"

namespace MySock {

typedef unsigned int SelectFlg;
const SelectFlg kSelectRead		= 0x1;
const SelectFlg kSelectWrite	= 0x2;
const SelectFlg kSelectExcept	= 0x4;
typedef std::map<SelectFlg, SOCKET_LIST> SelectResults;
typedef std::pair<SelectFlg, SOCKET_LIST> SelectResultsPair;

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

	void addSocket(SOCKET sock, SelectFlg flg);
	void removeSocket(SOCKET sock);
	void clearSockets();

public:
	void setTimeout(unsigned int timeout);

private:
	SelectSockets m_sockets;
	unsigned int m_timeout;
};

}
