// UDPServerSample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <winsock2.h>
#include <wincon.h>

#include <iostream>

#include <MySocketLib/AddrInfo.h>
#include <MySocketLib/UDPSocket.h>
#include <MySocketLib/SocketSelector.h>
#include <MySocketLib/MySockUtil.h>
#include <MySocketLib/MySockException.h>
#include <MyLib/String/StringUtil.h>

// 終了イベント
HANDLE g_exitEvent = NULL;
// 制御コード通知ハンドラ関数
BOOL WINAPI HandlerRoutine(DWORD ctrlType);

int _tmain(int argc, _TCHAR* argv[]) {

	// 終了イベント作成
	g_exitEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if(g_exitEvent == NULL) {
		std::wcout << _T("CreateEvent error=") << ::GetLastError() << std::endl;
		return -1;
	}
	// 制御コード通知ハンドラ追加
	if(!::SetConsoleCtrlHandler(&HandlerRoutine, TRUE)) {
		std::wcout << _T("SetConsoleCtrlHandler(set) error=") << ::GetLastError() << std::endl;
	}

	// WinSock 初期化
	WSADATA wsa = {0};
	int ret = ::WSAStartup(MAKEWORD(2, 2), &wsa);
	if(ret != 0) {
		std::wcout << _T("WSAStartup error=") << ret << std::endl;
		return -1;
	}

	try {
		// アドレス情報取得
		MySock::AddrInfoList addrInfos = MySock::getAddrInfoUDP(NULL, 60000, AI_PASSIVE, AF_UNSPEC);
		// 待ち受けソケット＆selectオブジェクト 作成
		MySock::UDPSocketList udpSockets;
		MySock::CSocketSelector selector;
		for(MySock::AddrInfoList::iterator it = addrInfos.begin(); it != addrInfos.end(); ++it) {
			MySock::CUDPSocket sock;
			// 待ち受けソケット作成
			sock.create(it->family());
			sock.bind(it->sockaddr());
			std::wcout << MySock::addressToString(&sock.getSockAddr().addr) << std::endl;
			// selectオブジェクトへ追加
			selector.addSocket(sock.socket(), MySock::kSelectRead | MySock::kSelectExcept);
			udpSockets.push_back(sock);
		}

		// データ受信ループ
		while(1) {
			// select
			MySock::SelectResults selectResult = selector.select();
			
			{	// except
				MySock::SOCKET_LIST exceptSockets = selectResult[MySock::kSelectExcept];
				for(MySock::SOCKET_LIST::iterator it = exceptSockets.begin(); it != exceptSockets.end(); ++it) {
					for(MySock::UDPSocketList::iterator itUdp = udpSockets.begin(); itUdp != udpSockets.end(); ++itUdp) {
						if(itUdp->socket() == (*it)) {
							std::wcout << _T("except socket ") << MySock::addressToString(&itUdp->getSockAddr().addr) << std::endl;
							break;
						}
					}
				}
			}
			{	// read
				MySock::SOCKET_LIST readSockets = selectResult[MySock::kSelectRead];
				for(MySock::SOCKET_LIST::iterator it = readSockets.begin(); it != readSockets.end(); ++it) {
					for(MySock::UDPSocketList::iterator itUdp = udpSockets.begin(); itUdp != udpSockets.end(); ++itUdp) {
						if(itUdp->socket() == (*it)) {
							MyLib::Data::BinaryData recvData;
							MySock::MySockAddr sockaddr = {0};
							itUdp->recv(recvData, &sockaddr);
							std::wcout << _T("!! arrived Data size=") << recvData.size() << std::endl <<
								("from=") << MySock::addressToString(&sockaddr.addr) << std::endl <<
								("to=") << MySock::addressToString(&itUdp->getSockAddr().addr) << std::endl <<
								MyLib::String::toHexStr(&recvData[0], recvData.size()) << std::endl;
							break;
						}
					}
				}
			}

			DWORD waitRet = ::WaitForSingleObject(g_exitEvent, 0);
			if(waitRet == WAIT_OBJECT_0) {
				std::wcout << _T("fire exit event") << std::endl;
				break;
			}  else if(waitRet == WAIT_ABANDONED) {
				std::wcout << _T("exit event abandoned") << std::endl;
				break;
			} else if(waitRet == WAIT_FAILED) {
				std::wcout << _T("wait exit error=") << ::GetLastError() << std::endl;
				break;
			} else {
				// WAIT_TIMEOUT
			}
		}

		// UDPサーバー 停止
		for(MySock::UDPSocketList::iterator it = udpSockets.begin(); it != udpSockets.end(); ++it) {
			it->close();
		}
//		udpServer.stop();
	} catch(MySock::CMySockException& e) {
		std::cout << e.what() << std::endl;
	}

	// WinSock 終了処理
	if(::WSACleanup() != 0) {
		std::wcout << _T("WSACleanup error=") << ::WSAGetLastError() << std::endl;
	}

	// 制御コード通知ハンドラ削除
	if(!::SetConsoleCtrlHandler(&HandlerRoutine, FALSE)) {
		std::wcout << _T("SetConsoleCtrlHandler(del) error=") << ::GetLastError() << std::endl;
	}
	// 終了イベント破棄
	if(!::CloseHandle(g_exitEvent)) {
		std::wcout << _T("CloseHandle error=") << ::GetLastError() << std::endl;
	}

	return 0;
}

BOOL WINAPI HandlerRoutine(DWORD ctrlType) {
	std::wcout << _T("on ctrl=") << ctrlType << std::endl;

	BOOL res = FALSE;
	switch(ctrlType) {
	// Ctrl+C, Ctrl+Breakは終了処理をする為にTRUEを返す
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
		if(g_exitEvent != NULL) {
			if(::SetEvent(g_exitEvent)) {
				res = TRUE;
			} else {
				std::wcout << _T("SetEvent err=") << ::GetLastError() << std::endl;
			}
		} else {
			std::wcout << _T("exit event is NULL") << std::endl;
		}
		break;
	// 上記以外(Close, ログオフ, シャットダウン)は強制終了させる
	default:
		res = FALSE;
		break;
	}
	return res;
}
