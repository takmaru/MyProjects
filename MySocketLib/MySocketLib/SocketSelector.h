#pragma once

#include <WinSock2.h>

#include <list>
#include <set>
#include <map>

#include "MySockTypedef.h"
#include "SocketBase.h"

namespace MySock {

typedef unsigned int SelectResult;
const SelectResult kResultConnectSuccess	= 1;
const SelectResult kResultConnectFailed		= 2;
const SelectResult kResultAcceptable		= 3;
const SelectResult kResultReadable			= 4;
const SelectResult kResultWriteable			= 5;
const SelectResult kResultExcept			= 6;
typedef std::map<SelectResult, MySock::SocketSet> SelectResults;
typedef std::pair<SelectResult, MySock::SocketSet> SelectResultsPair;

class CSocketSelector {
//private:
//	typedef std::map<SOCKET, SelectFlg> SelectSockets;
//	typedef std::pair<SOCKET, SelectFlg> SelectSocketsPair;
//	typedef std::set<CSocketBase*> SelectSockets;

public:
	CSocketSelector();
	explicit CSocketSelector(unsigned int timeout);
	~CSocketSelector();

public:
	SelectResults select();
	SelectResults select(unsigned int timeout);

	void addSocket(CSocketBase* sock);
	void removeSocket(CSocketBase* sock);
	void clearSockets();

public:
	void setTimeout(unsigned int timeout);

private:
	MySock::SocketSet m_sockets;
	unsigned int m_timeout;
};

}
