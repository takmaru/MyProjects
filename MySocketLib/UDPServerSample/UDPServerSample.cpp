// UDPServerSample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <winsock2.h>

#include <iostream>

#include <MySocketLib/UDP/UDPServer.h>
#include <MySocketLib/MySockException.h>

int _tmain(int argc, _TCHAR* argv[]) {
	WSADATA wsa = {0};
	int ret = ::WSAStartup(MAKEWORD(2, 2), &wsa);
	if(ret != 0) {
		std::wcout << _T("WSAStartup error=") << ret << std::endl;
		return -1;
	}

	MySock::CUDPServer udpServer;
	try {
		udpServer.start(60000);
		udpServer.stop();
	} catch(MySock::CMySockException& e) {
		std::cout << e.what() << std::endl;
	}

	if(::WSACleanup() != 0) {
		std::wcout << _T("WSACleanup error=") << ::WSAGetLastError() << std::endl;
	}

	return 0;
}

