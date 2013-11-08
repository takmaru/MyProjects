// TCPServerSample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <vector>
#include <algorithm>

#include <MyLib/tstring/tstring.h>
#include <MyLib/String/StringUtil.h>
#include <MyLib/Data/DataUtil.h>
#include <MySocketLib/AddrInfo.h>
#include <MySocketLib/TCPSocket.h>
#include <MySocketLib/SocketSelector.h>
#include <MySocketLib/MySockUtil.h>
#include <MySocketLib/MySockException.h>

namespace {
	// 終了イベント
	HANDLE g_exitEvent = NULL;
	// 制御コード通知ハンドラ関数
	BOOL WINAPI HandlerRoutine(DWORD ctrlType);

	typedef std::map<MySock::CSocketBase*, DWORD, MySock::compareSocketBase> SocketSendTimeMap;
	typedef std::pair<MySock::CSocketBase*, DWORD> SocketSendTimePair;

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
		MySock::AddrInfoList addrInfos = MySock::getAddrInfoTCP("S59009717", 60000, 0, AF_UNSPEC);

		// ソケットリスト
		MySock::SocketSet tcpSockets;
		// selectオブジェクト
		MySock::CSocketSelector selector;

		// TCPクライアントソケット作成
		for(MySock::AddrInfoList::iterator it = addrInfos.begin(); it != addrInfos.end(); ++it) {
			MySock::CTCPSocket* sock = new MySock::CTCPSocket();
			// TCPクライアントソケット作成
			sock->create(it->family());
			sock->setBlockingMode(false);
			sock->connect(it->sockaddr());
			tcpSockets.insert(sock);
			// selectオブジェクトへ追加
			selector.addSocket(sock);
		}

		SocketSendTimeMap socketSendtimes;
		bool isExit = false;
		while(CheckExitEvent(isExit)) {
			// 終了チェックでエラーなし（エラーがあればループ終了）

			if(isExit) {
				// 終了処理
				// 接続中のソケット全てシャットダウンする
				for(MySock::SocketSet::iterator it = tcpSockets.begin(); it != tcpSockets.end(); ++it) {
					MySock::CTCPSocket* sock = dynamic_cast<MySock::CTCPSocket*>(*it);
					if(sock->state() == MySock::kSockState_Connecting) {
						sock->shutdown(SD_SEND);
					}
				}
			}

			// select
			MySock::SelectResults selectResult = selector.select();

			{	// 接続失敗
				MySock::SocketSet sockets = selectResult[MySock::kResultConnectFailed];
				for(MySock::SocketSet::iterator it = sockets.begin(); it != sockets.end(); ++it) {
					std::tcout << _T("connect error sock=") << (*it)->socket() << _T(" err=") << (*it)->lastError() << std::endl;
				}
			}
			{	// 接続成功
				MySock::SocketSet sockets = selectResult[MySock::kResultConnectSuccess];
				for(MySock::SocketSet::iterator it = sockets.begin(); it != sockets.end(); ++it) {
					std::tcout << _T("connect success sock=") << (*it)->socket() << std::endl;
					socketSendtimes.insert(SocketSendTimePair((*it), 0));
				}
			}

			for(MySock::SocketSet::iterator it = tcpSockets.begin(); it != tcpSockets.end(); ++it) {
				MySock::CTCPSocket* sock = dynamic_cast<MySock::CTCPSocket*>(*it);
				if(sock->state() == MySock::kSockState_Connecting) {
					// 状態：接続中
					if(sock->isIOState(MySock::kSocketIOState_Writeable)) {
						// WriteフラグON
						SocketSendTimeMap::iterator itFind = socketSendtimes.find(sock);
						if(itFind != socketSendtimes.end()) {
							DWORD now = ::GetTickCount();
							if(now - itFind->second > 1000) {
								// 最後に送信してから特定時間以上経過していれば、データ送信
								sock->send(MyLib::Data::randomData(128));
								itFind->second = now;
							}
						} else {
							// ソケット最終接続時間マップに見つからなかった？
							std::tcout << _T("connected writeable socket not found ?") << std::endl;
						}
					}
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
						selector.removeSocket(sock);
						sock->close();
					}
				}
			}

			if(isExit) {
				// 終了処理
				bool isExistConnectingSocket = false;
				for(MySock::SocketSet::iterator it = tcpSockets.begin(); it != tcpSockets.end(); ++it) {
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

		// ソケットの破棄
		for(MySock::SocketSet::iterator it = tcpSockets.begin(); it != tcpSockets.end(); ++it) {
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