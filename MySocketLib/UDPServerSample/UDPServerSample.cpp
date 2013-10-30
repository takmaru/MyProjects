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
#include <MyLib/tstring/tstring.h>

// 終了イベント
HANDLE g_exitEvent = NULL;
// 制御コード通知ハンドラ関数
BOOL WINAPI HandlerRoutine(DWORD ctrlType);

int _tmain(int argc, _TCHAR* argv[]) {

	// 終了イベント作成
	g_exitEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if(g_exitEvent == NULL) {
		std::tcout << _T("CreateEvent error=") << ::GetLastError() << std::endl;
		return -1;
	}
	// 制御コード通知ハンドラ追加
	if(!::SetConsoleCtrlHandler(&HandlerRoutine, TRUE)) {
		std::tcout << _T("SetConsoleCtrlHandler(set) error=") << ::GetLastError() << std::endl;
	}

	// WinSock 初期化
	WSADATA wsa = {0};
	int ret = ::WSAStartup(MAKEWORD(2, 2), &wsa);
	if(ret != 0) {
		std::tcout << _T("WSAStartup error=") << ret << std::endl;
		return -1;
	}

	try {
		// アドレス情報取得
		MySock::AddrInfoList addrInfos = MySock::getAddrInfoUDP(NULL, 60000, AI_PASSIVE, AF_UNSPEC);
		// 待ち受けソケット＆selectオブジェクト 作成
		MySock::SocketSet udpSockets;
		MySock::CSocketSelector selector;
		for(MySock::AddrInfoList::iterator it = addrInfos.begin(); it != addrInfos.end(); ++it) {
			MySock::CUDPSocket* sock = new MySock::CUDPSocket();
			// 待ち受けソケット作成
			sock->create(it->family());
			sock->bind(it->sockaddr());
			std::wcout << MySock::addressToString(&(sock->getSockAddr().addr)) << std::endl;
			// selectオブジェクトへ追加
			udpSockets.insert(sock);
			selector.addSocket(sock);
		}

		// データ受信ループ
		while(1) {
			// select
			MySock::SelectResults selectResult = selector.select();
			
			{	// read
				MySock::SocketSet readSockets = selectResult[MySock::kResultReadable];
				for(MySock::SocketSet::iterator it = readSockets.begin(); it != readSockets.end(); ++it) {
					MySock::CUDPSocket* sock = dynamic_cast<MySock::CUDPSocket*>(*it);
					if(sock != NULL) {
						// 受信
						MyLib::Data::BinaryData recvData;
						MySock::MySockAddr sockaddr = {0};
						bool isSendError = false;
						sock->recv(recvData, &sockaddr, isSendError);
						if(!isSendError) {
							std::wcout << _T("!! arrived Data size=") << recvData.size() << std::endl <<
								("from=") << MySock::addressToString(&sockaddr.addr) << std::endl <<
								("to=") << MySock::addressToString(&sock->getSockAddr().addr) << std::endl <<
								MyLib::String::toHexStr(&recvData[0], recvData.size()) << std::endl;
							// そのまま返信
							sock->sendTo(sockaddr, recvData);
						} else {
							std::wcout << _T("!! last send data is Unreachabled") <<
								(" sock=") << MySock::addressToString(&sock->getSockAddr().addr) << std::endl;
						}
					} else {
						std::tcout << _T("socket protocol(") << (*it)->protocol() << _T(") isn't udp?") << std::endl;
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
		for(MySock::SocketSet::iterator it = udpSockets.begin(); it != udpSockets.end(); ++it) {
			(*it)->close();
			delete (*it);
		}

	} catch(MySock::CMySockException& e) {
		std::cout << e.what() << std::endl;
	}

	// WinSock 終了処理
	if(::WSACleanup() != 0) {
		std::tcout << _T("WSACleanup error=") << ::WSAGetLastError() << std::endl;
	}

	// 制御コード通知ハンドラ削除
	if(!::SetConsoleCtrlHandler(&HandlerRoutine, FALSE)) {
		std::tcout << _T("SetConsoleCtrlHandler(del) error=") << ::GetLastError() << std::endl;
	}
	// 終了イベント破棄
	if(!::CloseHandle(g_exitEvent)) {
		std::tcout << _T("CloseHandle error=") << ::GetLastError() << std::endl;
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
