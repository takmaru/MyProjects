#pragma once

#include <ws2def.h>
#include <string>
#include <list>

#include <MyLib/tstring/tstring.h>

#include "MySockTypedef.h"

namespace MySock {

class CAddrInfo {
public:
	CAddrInfo();
	explicit CAddrInfo(const ADDRINFOA& addrInfo);
	CAddrInfo(int flags, int family, int socktype, int protocol);
	~CAddrInfo();
public:
	operator ADDRINFOA();

public:
	int flags() const {
		return m_flags;
	}
	int family() const {
		return m_family;
	}
	int socktype() const {
		return m_socktype;
	}
	int protocol() const {
		return m_protocol;
	}
	std::string name() const {
		return m_name;
	}
	MySock::MySockAddr sockaddr() const {
		return m_sockaddr;
	}

	std::tstring descpription() const;

private:
	int m_flags;
	int m_family;
	int m_socktype;
	int m_protocol;
	std::string m_name;
	int m_sockaddrlen;
	MySock::MySockAddr m_sockaddr;
};
typedef std::list<CAddrInfo> AddrInfoList;

AddrInfoList getAddrInfo(const char* host, unsigned short port, int flags, int family, int socktype, int protocol);
AddrInfoList getAddrInfo(const char* host, const char* service, int flags, int family, int socktype, int protocol);
AddrInfoList getAddrInfo(const char* host, unsigned short port, const CAddrInfo& hint);
AddrInfoList getAddrInfo(const char* host, const char* service, const CAddrInfo& hint);
AddrInfoList getAddrInfo(const char* host, unsigned short port, const ADDRINFOA& hint);
AddrInfoList getAddrInfo(const char* host, const char* service, const ADDRINFOA& hint);

AddrInfoList getAddrInfoUDPClient(const char* host, int flags, int family);

}
