// UDPClientSample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <winsock2.h>

#include <iostream>

#include <MySocketLib/UDP/UDPClient.h>
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

	MySock::CUDPClient udpClient;
	try {
		if(udpClient.send("S59009717", 60000, MyLib::Data::randomData(128))) {
			std::wcout << _T("send success") << std::endl;
		} else {
			std::wcout << _T("send error") << std::endl;
			MySock::MYSOCKERRORS errors = udpClient.send_errors();
			for(MySock::MYSOCKERRORS::iterator it = errors.begin(); it != errors.end(); ++it) {
				std::wcout << _T("type=") << (*it).type << _T(" error=") << (*it).error << std::endl;
			}
		}
	} catch(MySock::CMySockException& e) {
		std::cout << e.what() << std::endl;
	}

	if(::WSACleanup() != 0) {
		std::wcout << _T("WSACleanup error=") << ::WSAGetLastError() << std::endl;
	}

	return 0;
}

