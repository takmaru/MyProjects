// UDPClientSample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <winsock2.h>

#include <iostream>

#include <MySocketLib/AddrInfo.h>
#include <MySocketLib/UDPSocket.h>
#include <MySocketLib/MySockException.h>
#include <MyLib/Data/BinaryData.h>
#include <MyLib/Data/DataUtil.h>

int _tmain(int argc, _TCHAR* argv[]) {
	WSADATA wsa = {0};
	int ret = ::WSAStartup(MAKEWORD(2, 2), &wsa);
	if(ret != 0) {
		std::wcout << _T("WSAStartup error=") << ret << std::endl;
		return -1;
	}

	try {
		// サーバーのアドレス情報取得
		MySock::AddrInfoList addrInfos = MySock::getAddrInfoUDP("S59009717", 60000, 0, AF_UNSPEC);
		if(addrInfos.size() > 0) {
			MySock::CAddrInfo& addrinfo = addrInfos[0];
			MySock::CUDPSocket socket;
			socket.create(addrinfo.family());
			socket.sendTo(addrinfo.sockaddr(), MyLib::Data::randomData(128));
			socket.close();
		} else {
			std::wcout << _T("addrinfo is none") << std::endl;
		}
	} catch(MySock::CMySockException& e) {
		std::cout << e.what() << std::endl;
	}

	if(::WSACleanup() != 0) {
		std::wcout << _T("WSACleanup error=") << ::WSAGetLastError() << std::endl;
	}

	return 0;
}

