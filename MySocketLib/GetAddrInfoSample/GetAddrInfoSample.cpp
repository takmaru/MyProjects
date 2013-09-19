// GetAddrInfoSample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <sstream>

#include <MyLib/tstring/tstring.h>
#include <MyLib/String/StringUtil.h>

#include <MySocketLib/AddrInfo.h>
#include <MySocketLib/MySockException.h>

void outputAddrInfos(const char* host, unsigned short port, int flags, int family, int socktype, int protocol);
void outputAddrInfos(const char* host, const char* service, int flags, int family, int socktype, int protocol);

int _tmain(int argc, _TCHAR* argv[]) {

	WSADATA wsa = {0};
	int ret = ::WSAStartup(MAKEWORD(2, 2), &wsa);
	if(ret != 0) {
		std::wcout << _T("WSAStartup error=") << ret << std::endl;
		return -1;
	}

	MySock::CAddrInfo addrInfo;
	try {
		outputAddrInfos("S59009717", 60000, 0, AF_UNSPEC, SOCK_DGRAM, IPPROTO_UDP);

		std::tcout << _T("// -- host NULL -------- //") << std::endl;
		outputAddrInfos(NULL, 60000, 0, AF_UNSPEC, SOCK_DGRAM, IPPROTO_UDP);
		std::tcout << _T("// -- service NULL -------- //") << std::endl;
		outputAddrInfos("S59009717", (char*)NULL, 0, AF_UNSPEC, SOCK_DGRAM, IPPROTO_UDP);

		// 名前解決できないホストは例外
		std::tcout << _T("// -- unknown ip -------- //") << std::endl;
		outputAddrInfos("10.2.3.4", 60000, 0, AF_UNSPEC, SOCK_DGRAM, IPPROTO_UDP);

		std::tcout << _T("// -- IPv4 -------- //") << std::endl;
		outputAddrInfos("S59009717", (char*)NULL, 0, AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		std::tcout << _T("// -- IPv6 -------- //") << std::endl;
		outputAddrInfos("S59009717", (char*)NULL, 0, AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

		std::tcout << _T("// -- My Mac -------- //") << std::endl;
		outputAddrInfos("S59009905", (char*)NULL, 0, AF_UNSPEC, SOCK_DGRAM, IPPROTO_UDP);
	} catch(MySock::CMySockException& e) {
		std::cout << e.what() << std::endl;
	}

	if(::WSACleanup() != 0) {
		std::wcout << _T("WSACleanup error=") << ::WSAGetLastError() << std::endl;
	}

	return 0;
}

void outputAddrInfos(const char* host, unsigned short port, int flags, int family, int socktype, int protocol) {
	std::ostringstream oss;
	oss << port;
	outputAddrInfos(host, oss.str().c_str(), flags, family, socktype, protocol);
}

void outputAddrInfos(const char* host, const char* service, int flags, int family, int socktype, int protocol) {
	MySock::AddrInfoList infos = MySock::getAddrInfo(host, service, flags, family, socktype, protocol);
	std::tcout << _T("hint");
	if(host != NULL) {
		std::tcout << _T(" host=") << MyLib::String::toWideChar(host);
	} else {
		std::tcout << _T(" host=NULL");
	}
	if(service != NULL) {
		std::tcout << _T(" service=") << MyLib::String::toWideChar(service);
	} else {
		std::tcout << _T(" service=NULL");
	}
	std::tcout <<
		_T(" flags=") << flags <<
		_T(" family=") << family <<
		_T(" socktype=") << socktype <<
		_T(" protocol=") << protocol <<
		std::endl;

	std::tcout << _T("AddrInfo count=") << infos.size() << std::endl;
	for(MySock::AddrInfoList::const_iterator it = infos.begin(); it != infos.end(); ++it) {
		std::tcout << it->descpription() << std::endl;
	}
}
