#include "stdafx.h"
#include "AddrInfo.h"

#include <WS2tcpip.h>

#include <sstream>

#include <MyLib/String/StringUtil.h>

#include "MySockException.h"
#include "MySockUtil.h"

MySock::CAddrInfo::CAddrInfo():
	m_flags(0), m_family(AF_UNSPEC), m_socktype(SOCK_STREAM), m_protocol(IPPROTO_IP),
	m_name(), m_sockaddrlen(0), m_sockaddr() {
}
MySock::CAddrInfo::CAddrInfo(const ADDRINFOA& addrInfo):
	m_flags(addrInfo.ai_flags), m_family(addrInfo.ai_family), m_socktype(addrInfo.ai_socktype), m_protocol(addrInfo.ai_protocol),
	m_name(), m_sockaddrlen(addrInfo.ai_addrlen), m_sockaddr() {
	// name
	if(addrInfo.ai_canonname != NULL) {
		m_name = addrInfo.ai_canonname;
	}
	// addr
	if(m_sockaddrlen == sizeof(SOCKADDR)) {
		m_sockaddr.addr = *addrInfo.ai_addr;
	} else if(m_sockaddrlen == sizeof(SOCKADDR_IN)) {
		m_sockaddr.v4 = *reinterpret_cast<SOCKADDR_IN*>(addrInfo.ai_addr);
	} else if(m_sockaddrlen == sizeof(SOCKADDR_IN6)) {
		m_sockaddr.v6 = *reinterpret_cast<SOCKADDR_IN6*>(addrInfo.ai_addr);
	}
}
MySock::CAddrInfo::CAddrInfo(int flags, int family, int socktype, int protocol):
	m_flags(flags), m_family(family), m_socktype(socktype), m_protocol(protocol),
	m_name(), m_sockaddrlen(0), m_sockaddr() {
}
MySock::CAddrInfo::~CAddrInfo() {
}

MySock::CAddrInfo::operator ADDRINFOA() {
	ADDRINFOA addrInfo = {0};
	addrInfo.ai_flags = m_flags;
	addrInfo.ai_family = m_family;
	addrInfo.ai_socktype = m_socktype;
	addrInfo.ai_protocol = m_protocol;
	addrInfo.ai_canonname = &m_name[0];
	if(m_name.size() == 0) {
		addrInfo.ai_canonname = NULL;
	}
	addrInfo.ai_addrlen = m_sockaddrlen;
	addrInfo.ai_addr = &m_sockaddr.addr;
	if(addrInfo.ai_addrlen == 0) {
		addrInfo.ai_addr = NULL;
	}
	return addrInfo;
}

std::tstring MySock::CAddrInfo::descpription() const {

	std::tstring name;
	if(sizeof(TCHAR) != sizeof(char)) {
		name = MyLib::String::toWideChar(m_name.c_str());
	} else {
		name = reinterpret_cast<const TCHAR*>(m_name.c_str());
	}

	std::tostringstream oss;
	oss << _T("CAddrInfo")
		_T(" flags=") << m_flags <<
		_T(" family=") << m_family <<
		_T(" socktype=") << m_socktype <<
		_T(" protocol=") << m_protocol <<
		_T(" name=") << name <<
		_T(" sockaddr=") << MySock::addressToString((const PSOCKADDR)&m_sockaddr.addr);
	return oss.str();
}

// -----

MySock::AddrInfoList MySock::getAddrInfo(const char* host, unsigned short port, int flags, int family, int socktype, int protocol) {
	std::ostringstream oss;
	oss << port;
	return MySock::getAddrInfo(host, oss.str().c_str(), CAddrInfo(flags, family, socktype, protocol));
}
MySock::AddrInfoList MySock::getAddrInfo(const char* host, const char* service, int flags, int family, int socktype, int protocol) {
	return MySock::getAddrInfo(host, service, CAddrInfo(flags, family, socktype, protocol));
}
MySock::AddrInfoList MySock::getAddrInfo(const char* host, unsigned short port, const CAddrInfo& hint) {
	std::ostringstream oss;
	oss << port;
	CAddrInfo copy(hint);
	return MySock::getAddrInfo(host, oss.str().c_str(), static_cast<ADDRINFOA>(copy));
}
MySock::AddrInfoList MySock::getAddrInfo(const char* host, const char* service, const CAddrInfo& hint) {
	CAddrInfo copy(hint);
	return MySock::getAddrInfo(host, service, static_cast<ADDRINFOA>(copy));
}
MySock::AddrInfoList MySock::getAddrInfo(const char* host, unsigned short port, const ADDRINFOA& hint) {
	std::ostringstream oss;
	oss << port;
	return MySock::getAddrInfo(host, oss.str().c_str(), hint);
}
MySock::AddrInfoList MySock::getAddrInfo(const char* host, const char* service, const ADDRINFOA& hint) {
	PADDRINFOA addrResults = NULL;
	int ret = ::getaddrinfo(host, service, &hint, &addrResults);
	if(ret != 0) {
		RAISE_MYSOCKEXCEPTION("[getAddrInfo] getaddrinfo ret=%d err=%d", ret, ::WSAGetLastError());
	}
	MySock::AddrInfoList results;
	for(PADDRINFOA p = addrResults; p != NULL; p = p->ai_next) {
		results.push_back(MySock::CAddrInfo(*p));
	}
	::freeaddrinfo(addrResults);
	return results;
}

MySock::AddrInfoList MySock::getAddrInfoUDP(const char* host, unsigned short port, int flags, int family) {
	std::ostringstream oss;
	oss << port;
	return MySock::getAddrInfoUDP(host, oss.str().c_str(), flags, family);
}
MySock::AddrInfoList MySock::getAddrInfoUDP(const char* host, const char* service, int flags, int family) {
	return MySock::getAddrInfo(host, service, flags, family, SOCK_DGRAM, IPPROTO_UDP);
}
