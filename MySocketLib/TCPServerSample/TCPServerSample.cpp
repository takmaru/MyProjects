// TCPServerSample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <algorithm>

#include <MyLib/tstring/tstring.h>
#include <MyLib/String/StringUtil.h>
#include <MySocketLib/AddrInfo.h>
#include <MySocketLib/TCPSocket.h>
#include <MySocketLib/SocketSelector.h>
#include <MySocketLib/MySockTypedef.h>
#include <MySocketLib/MySockUtil.h>
#include <MySocketLib/MySockException.h>

namespace {

// 終了イベント
HANDLE g_exitEvent = NULL;
// 制御コード通知ハンドラ関数
BOOL WINAPI HandlerRoutine(DWORD ctrlType);

void OnReadable(MySock::CTCPSocket& sock);
bool CheckExitEvent(bool& isExit);
};


int _tmain(int argc, _TCHAR* argv[]) {

	// 終了イベント作成
	g_exitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
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
		MySock::AddrInfoList addrInfos = MySock::getAddrInfoTCP(NULL, 60000, AI_PASSIVE, AF_UNSPEC);
		// 待ち受けソケット＆selectオブジェクト 作成
		MySock::SocketSet listenSockets;
		MySock::CSocketSelector selector;
		for(MySock::AddrInfoList::iterator it = addrInfos.begin(); it != addrInfos.end(); ++it) {
			MySock::CTCPSocket* sock = new MySock::CTCPSocket();
			// 待ち受けソケット作成
			sock->create(it->family());
			sock->bind(it->sockaddr());
			sock->listen();
			listenSockets.insert(sock);
			std::wcout << MySock::addressToString(&sock->getSockAddr().addr) << std::endl;
			// selectオブジェクトへ追加
			selector.addSocket(sock);
		}

		MySock::SocketSet acceptSockets;
		bool isExit = false;
		while(CheckExitEvent(isExit)) {
			// 終了チェックでエラーなし（エラーがあればループ終了）

			if(isExit) {
				// 終了処理
				// 接続中のソケット全てシャットダウンする
				for(MySock::SocketSet::iterator it = acceptSockets.begin(); it != acceptSockets.end(); ++it) {
					MySock::CTCPSocket* sock = dynamic_cast<MySock::CTCPSocket*>(*it);
					if(sock->state() == MySock::kSockState_Connecting) {
						sock->shutdown(SD_SEND);
					}
				}
				// 待ち受けソケットは全て閉じて破棄
				for(MySock::SocketSet::iterator it = listenSockets.begin(); it != listenSockets.end(); ++it) {
					MySock::CTCPSocket* sock = dynamic_cast<MySock::CTCPSocket*>(*it);
					selector.removeSocket(sock);
					sock->close();
					delete sock;
				}
				listenSockets.clear();
			}

			// select
			MySock::SelectResults selectResult = selector.select();

			{	// 接続許可
				MySock::SocketSet sockets = selectResult[MySock::kResultAcceptable];
				for(MySock::SocketSet::iterator it = sockets.begin(); it != sockets.end(); ++it) {
					MySock::CTCPSocket* accept = new MySock::CTCPSocket(dynamic_cast<MySock::CTCPSocket*>(*it)->accept());
					acceptSockets.insert(accept);
					selector.addSocket(accept);
					std::wcout << _T("accept sock=") << accept->socket() << std::endl <<
						_T(" my=") << MySock::addressToString(&(accept->getSockAddr().addr)) << std::endl <<
						_T(" peer=") << MySock::addressToString(&(accept->getPeerAddr().addr)) << std::endl;
				}
			}

			// 通信ソケット
			for(MySock::SocketSet::iterator it = acceptSockets.begin(); it != acceptSockets.end(); ++it) {
				MySock::CTCPSocket* sock = dynamic_cast<MySock::CTCPSocket*>(*it);
				if(sock->state() == MySock::kSockState_Connecting) {
					// 状態：接続中
					if(sock->isIOState(MySock::kSocketIOState_Readable)) {
						// ReadフラグON
						// データ受信処理
						OnReadable(*sock);
					}
				} else if(sock->state() == MySock::kSockState_GracefulClosing) {
					// 状態：クローズ処理中
					if(	sock->isIOState(MySock::kSocketIOState_RecvFin) &&
						!sock->isIOState(MySock::kSocketIOState_SendFin)	) {
						// Fin受信、Fin未送信
						// シャットダウン
						sock->shutdown(SD_SEND);
					} else
					if(	!sock->isIOState(MySock::kSocketIOState_RecvFin) &&
						sock->isIOState(MySock::kSocketIOState_SendFin)	){
						// Fin送信、Fin未受信
						if(sock->isIOState(MySock::kSocketIOState_Readable)) {
							// ReadフラグON
							// データ受信処理
							OnReadable(*sock);
						}
					}

					if(sock->isIOState(MySock::kSocketIOState_FinBoth)) {
						// Finを送信し、Finを受信しているなら終了処理
						sock->close();
					}
				}
			}

			// クローズ済みのソケットをリストから削除
//			std::remove_if(acceptSockets.begin(), acceptSockets.begin(), );


			if(isExit) {
				// 終了処理
				bool isExistConnectingSocket = false;
				for(MySock::SocketSet::iterator it = acceptSockets.begin(); it != acceptSockets.end(); ++it) {
					MySock::CTCPSocket* sock = dynamic_cast<MySock::CTCPSocket*>(*it);
					if(	(sock->state() != MySock::kSockState_Created) && 
						(sock->state() != MySock::kSockState_Closed)	) {
						isExistConnectingSocket = true;
						break;
					}
				}
				if(!isExistConnectingSocket) {
					// 全てのソケットが接続失敗（作成後）か接続中ソケットだけであればループ終了
					break;
				}
			}
		}

		// 接続済みソケット 停止
		for(MySock::SocketSet::iterator it = acceptSockets.begin(); it != acceptSockets.end(); ++it) {
			MySock::CTCPSocket* sock = dynamic_cast<MySock::CTCPSocket*>(*it);
			if(sock->state() != MySock::kSockState_Closed) {
				sock->close();
			}
			delete sock;
		}
		// TCPサーバー 停止
		for(MySock::SocketSet::iterator it = listenSockets.begin(); it != listenSockets.end(); ++it) {
			MySock::CTCPSocket* sock = dynamic_cast<MySock::CTCPSocket*>(*it);
			if(sock->state() != MySock::kSockState_Closed) {
				sock->close();
			}
			delete sock;
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

namespace {

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

void OnReadable(MySock::CTCPSocket& sock) {
	bool isFinRecv = false;
	MyLib::Data::BinaryData recvData = sock.recv(&isFinRecv);
	if(!isFinRecv) {
		// データ受信
		std::wcout << _T("recv data size=") << recvData.size() << std::endl <<
			_T(" my=") << MySock::addressToString(&(sock.getSockAddr().addr)) << std::endl <<
			_T(" peer=") << MySock::addressToString(&(sock.getPeerAddr().addr)) << std::endl <<
			MyLib::String::toHexStr(&recvData[0], recvData.size()) << std::endl;
	} else {
		// Fin受信
		std::tcout << _T("Fin recv sock=") << sock.socket() << std::endl;
	}
}

bool CheckExitEvent(bool& isExit) {
	// 終了確認
	DWORD waitRet = ::WaitForSingleObject(g_exitEvent, 0);
	if(waitRet == WAIT_ABANDONED) {
		std::tcout << _T("exit event abandoned") << std::endl;
		return false;
	} else if(waitRet == WAIT_FAILED) {
		std::tcout << _T("wait exit error=") << ::GetLastError() << std::endl;
		return false;
	} else if(waitRet == WAIT_OBJECT_0) {
		isExit = true;
	} // else WAIT_TIMEOUT
	return true;
}

}