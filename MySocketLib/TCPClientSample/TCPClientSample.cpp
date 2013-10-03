// TCPServerSample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <MyLib/tstring/tstring.h>
#include <MySocketLib/AddrInfo.h>
#include <MySocketLib/TCPSocket.h>
#include <MySocketLib/SocketSelector.h>
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
		MySock::AddrInfoList addrInfos = MySock::getAddrInfoTCP("S59009717", 60000, 0, AF_UNSPEC);
		// TCPクライアントソケット作成＆接続時selectオブジェクト 作成
		MySock::TCPSocketList connectSockets;
		MySock::CSocketSelector connectSelector;
		for(MySock::AddrInfoList::iterator it = addrInfos.begin(); it != addrInfos.end(); ++it) {
			MySock::CTCPSocket sock;
			// TCPクライアントソケット作成
			sock.create(it->family());
			sock.setBlockingMode(false);
			sock.connect(it->sockaddr());
			// selectオブジェクトへ追加
			connectSelector.addSocket(sock.socket(), MySock::kSelectWrite | MySock::kSelectExcept);
			connectSockets.push_back(sock);
		}

		// 接続待ちループ
		MySock::TCPSocketList connectedSockets;
		while(!connectSockets.empty()) {
			// select
			MySock::SelectResults selectResult = connectSelector.select();

			{	// except
				MySock::SOCKET_LIST exceptSockets = selectResult[MySock::kSelectExcept];
				for(MySock::SOCKET_LIST::iterator it = exceptSockets.begin(); it != exceptSockets.end(); ++it) {
					for(MySock::TCPSocketList::iterator itTcp = connectSockets.begin(); itTcp != connectSockets.end(); ++itTcp) {
						if(itTcp->socket() == (*it)) {
							std::wcout << _T("except socket") << std::endl;
							itTcp->close();
							connectSockets.erase(itTcp);
							break;
						}
					}
				}
			}
			{	// write
				MySock::SOCKET_LIST writeSockets = selectResult[MySock::kSelectWrite];
				for(MySock::SOCKET_LIST::iterator it = writeSockets.begin(); it != writeSockets.end(); ++it) {
					for(MySock::TCPSocketList::iterator itTcp = connectSockets.begin(); itTcp != connectSockets.end(); ++itTcp) {
						if(itTcp->socket() == (*it)) {
							// connected
							std::wcout << _T("connect socket ") << MySock::addressToString(&itTcp->getSockAddr().addr) <<
								_T(" to ") << MySock::addressToString(&itTcp->getPeerAddr().addr) << std::endl;
							connectedSockets.push_back(*itTcp);
							connectSockets.erase(itTcp);
							break;
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
		for(MySock::TCPSocketList::iterator it = connectedSockets.begin(); it != connectedSockets.end(); ++it) {
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
