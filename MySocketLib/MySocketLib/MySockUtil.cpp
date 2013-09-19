#include "stdafx.h"

#include <tchar.h>
#include <vector>

#include "MySockUtil.h"

std::tstring MySock::addressToString(const PSOCKADDR sockaddr) {

	// IPv4, IPv6‚Ì”»’f
	DWORD addrLen = sizeof(SOCKADDR_IN);
	if(sockaddr->sa_family == AF_INET6) {
		addrLen = sizeof(SOCKADDR_IN6);
	}

	// •¶Žš—ñ’·Žæ“¾
	DWORD addrStrLength = 0;
	::WSAAddressToString(sockaddr, addrLen, NULL, NULL, &addrStrLength);
	if(addrStrLength == 0) {
		return std::tstring();
	}

	// •ÏŠ·•¶Žš—ñŽæ“¾
	std::vector<TCHAR> addrStr(addrStrLength, 0);
	if(::WSAAddressToString(sockaddr, addrLen, NULL, &addrStr[0], &addrStrLength) != 0) {
		return std::tstring();
	}

	return std::tstring(&addrStr[0]);
}

std::tstring MySock::ntop(const PSOCKADDR sockaddr) {
	std::vector<TCHAR> addrBuf;
	if(sockaddr->sa_family == AF_INET) {
		IN_ADDR inAddr = reinterpret_cast<PSOCKADDR_IN>(sockaddr)->sin_addr;
		addrBuf.resize(16, 0);
		if(::InetNtop(AF_INET, &inAddr, &addrBuf[0], addrBuf.size()) == NULL) {
			return std::tstring();
		}
	} else if(sockaddr->sa_family == AF_INET6) {
		IN6_ADDR inAddr = reinterpret_cast<PSOCKADDR_IN6>(sockaddr)->sin6_addr;
		addrBuf.resize(46, 0);
		if(::InetNtop(AF_INET6, &inAddr, &addrBuf[0], addrBuf.size()) == NULL) {
			return std::tstring();
		}
	}
	return std::tstring(reinterpret_cast<PTCHAR>(&addrBuf[0]));
}
