// UDPClientConnectSample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <MySocketLib/AddrInfo.h>
#include <MySocketLib/UDPSocket.h>
#include <MySocketLib/UDP/UDPClient.h>
#include <MySocketLib/MySockException.h>
#include <MyLib/Data/BinaryData.h>
#include <MyLib/Data/DataUtil.h>
#include <MyLib/tstring/tstring.h>


int _tmain(int argc, _TCHAR* argv[]) {
	WSADATA wsa = {0};
	int ret = ::WSAStartup(MAKEWORD(2, 2), &wsa);
	if(ret != 0) {
		std::wcout << _T("WSAStartup error=") << ret << std::endl;
		return -1;
	}

	try {
		MySock::AddrInfoList addrInfos = MySock::getAddrInfoUDPClient("S59009717", 0, AF_UNSPEC);
		for(MySock::AddrInfoList::iterator it = addrInfos.begin(); it != addrInfos.end(); ++it) {
			MySock::CUDPSocket socket;
			socket.create(it->family());

			socket.close();
		}
	} catch(MySock::CMySockException& e) {
		std::cout << e.what() << std::endl;
	}
/*
	MySock::CUDPClient udpClient;
	try {
		udpClient.connect("S59009717", 60000);
//		udpClient.connect("1.1.1.1", 60000);
		std::wcout << _T("connect") << std::endl;
//-->
		if(udpClient.send(MyLib::Data::randomData(128))) {
			std::wcout << _T("send success") << std::endl;
		} else {
			std::wcout << _T("send error") << std::endl;
			MySock::MYSOCKERRORS errors = udpClient.sock_errors();
			for(MySock::MYSOCKERRORS::iterator it = errors.begin(); it != errors.end(); ++it) {
				std::wcout << _T("type=") << (*it).type << _T(" error=") << (*it).error << std::endl;
			}
		}
//<--
		udpClient.disconnect();
		std::wcout << _T("disconnect") << std::endl;
	} catch(MySock::CMySockException& e) {
		std::cout << e.what() << std::endl;
		MySock::MYSOCKERRORS errors = udpClient.sock_errors();
		for(MySock::MYSOCKERRORS::iterator it = errors.begin(); it != errors.end(); ++it) {
			std::wcout << _T("type=") << (*it).type << _T(" error=") << (*it).error << std::endl;
		}
	}
*/
	if(::WSACleanup() != 0) {
		std::wcout << _T("WSACleanup error=") << ::WSAGetLastError() << std::endl;
	}

	return 0;
}
