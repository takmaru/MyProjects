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

namespace {
	typedef unsigned int SocketState;
	const SocketState kSockState_WaitConnect	= 0x00000001;
	const SocketState kSockState_Connecting		= 0x00000002;
	const SocketState kSockState_Readable		= 0x00000004;
	const SocketState kSockState_Writeable		= 0x00000008;
	const SocketState kSockState_Closing		= 0x00000010;
	const SocketState kSockState_Closed			= 0x00000020;
	const SocketState kSockState_Listening		= 0x00000040;

	class CTCPSocketStateful : public MySock::CTCPSocket {
	public:
		CTCPSocketStateful() : MySock::CTCPSocket(),
			m_state(0), m_lastDataSendTickCount(0) {
		}
		CTCPSocketStateful(const CTCPSocketStateful& obj) : MySock::CTCPSocket(obj),
			m_state(0), m_lastDataSendTickCount(0) {
		}
		CTCPSocketStateful(SOCKET sock, int family) : MySock::CTCPSocket(sock, family),
			m_state(0), m_lastDataSendTickCount(0) {
		}
		virtual ~CTCPSocketStateful() {
		}
	public:
		void setState(SocketState state) {
			m_state |= state;
		}
		void resetState(SocketState state) {
			m_state &= ~state;
		}
		bool isSetState(SocketState state) const {
			return ((m_state & state) != 0);
		}
		SocketState state() const {
			return m_state;
		}
		bool canSend() const {
			return (	(m_lastDataSendTickCount == 0) ||
						((::GetTickCount() - m_lastDataSendTickCount) > 1000)	);
		}
		void setSendTickCount(DWORD tickCount) {
			m_lastDataSendTickCount = tickCount;
		}
	private:
		SocketState m_state;
		DWORD m_lastDataSendTickCount;
	};
	typedef std::vector<CTCPSocketStateful> StatefulTCPSocketList;
	class IsClosedSocket {
	public:
		bool operator()(const CTCPSocketStateful& sock) {
			return sock.isSetState(kSockState_Closed);
		}
	};
};

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
		MySock::AddrInfoList addrInfos = MySock::getAddrInfoTCP(NULL, 60000, AI_PASSIVE, AF_UNSPEC);
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
			selector.addSocket(sock.socket(), MySock::kSelectRead);
			listenSockets.push_back(sock);
		}

		// 接続待ち受けループ
		MySock::TCPSocketList acceptSockets;
		while(1) {
			// select
			MySock::SelectResults selectResult = selector.select();

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
