// TCPServerSample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <MyLib/tstring/tstring.h>
#include <MySocketLib/AddrInfo.h>
#include <MySocketLib/TCPSocket.h>
#include <MySocketLib/SocketSelector.h>
#include <MySocketLib/MySockTypedef.h>
#include <MySocketLib/MySockUtil.h>
#include <MySocketLib/MySockException.h>

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
		MySock::TCPSocketList listenSockets;
		MySock::CSocketSelector selector;
		for(MySock::AddrInfoList::iterator it = addrInfos.begin(); it != addrInfos.end(); ++it) {
			MySock::CTCPSocket sock;
			// 待ち受けソケット作成
			sock.create(it->family());
			sock.bind(it->sockaddr());
			sock.listen();
			std::wcout << MySock::addressToString(&sock.getSockAddr().addr) << std::endl;
			// selectオブジェクトへ追加
			selector.addSocket(sock.socket(), MySock::kSelectRead | MySock::kSelectExcept);
			listenSockets.push_back(sock);
		}

		// 接続待ち受けループ
		MySock::TCPSocketList acceptSockets;
		while(1) {
			// select
			MySock::SelectResults selectResult = selector.select();

			{	// except
				MySock::SOCKET_LIST exceptSockets = selectResult[MySock::kSelectExcept];
				for(MySock::SOCKET_LIST::iterator it = exceptSockets.begin(); it != exceptSockets.end(); ++it) {
					for(MySock::TCPSocketList::iterator itTcp = listenSockets.begin(); itTcp != listenSockets.end(); ++itTcp) {
						if(itTcp->socket() == (*it)) {
							std::wcout << _T("except socket ") << MySock::addressToString(&itTcp->getSockAddr().addr) << std::endl;
							break;
						}
					}
				}
			}
			{	// read
				MySock::SOCKET_LIST readSockets = selectResult[MySock::kSelectRead];
				for(MySock::SOCKET_LIST::iterator it = readSockets.begin(); it != readSockets.end(); ++it) {
					for(MySock::TCPSocketList::iterator itTcp = listenSockets.begin(); itTcp != listenSockets.end(); ++itTcp) {
						if(itTcp->socket() == (*it)) {
							// accept
							MySock::MySockAddr sockaddr = {0};
							acceptSockets.push_back(itTcp->accept(&sockaddr));
							MySock::CTCPSocket& sock = acceptSockets.back();
							std::wcout << _T("accept socket ") << MySock::addressToString(&sock.getSockAddr().addr) <<
								_T(" from ") << MySock::addressToString(&sock.getPeerAddr().addr) <<
								_T(" accept ") << MySock::addressToString(&sockaddr.addr) << std::endl;
						}
					}
				}
			}

			// 終了確認
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

		// 接続済みソケット 停止
		for(MySock::TCPSocketList::iterator it = acceptSockets.begin(); it != acceptSockets.end(); ++it) {
			it->close();
		}
		// TCPサーバー 停止
		for(MySock::TCPSocketList::iterator it = listenSockets.begin(); it != listenSockets.end(); ++it) {
			it->close();
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
