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
	typedef unsigned int SocketState;
	const SocketState kSockState_WaitConnect	= 0x00000001;
	const SocketState kSockState_Connecting		= 0x00000002;
	const SocketState kSockState_Readable		= 0x00000004;
	const SocketState kSockState_Writeable		= 0x00000008;
	const SocketState kSockState_Closing		= 0x00000010;
	const SocketState kSockState_Closed			= 0x00000020;

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
		MySock::AddrInfoList addrInfos = MySock::getAddrInfoTCP("S59009717", 60000, 0, AF_UNSPEC);

		// ソケットリスト
		StatefulTCPSocketList tcpSockets;

		// TCPクライアントソケット作成
		for(MySock::AddrInfoList::iterator it = addrInfos.begin(); it != addrInfos.end(); ++it) {
			CTCPSocketStateful sock;
			// TCPクライアントソケット作成
			sock.create(it->family());
			sock.setBlockingMode(false);
			sock.connect(it->sockaddr());
			sock.setState(kSockState_WaitConnect);
			tcpSockets.push_back(sock);
		}

		// selectオブジェクト
		MySock::CSocketSelector selector;
		// 接続待ちループ
		bool isNeedUpdateSelectSocket = true;
		while(1) {
			if(isNeedUpdateSelectSocket) {
				// selectオブジェクトが持つソケット情報をクリア
				selector.clearSockets();
				for(StatefulTCPSocketList::iterator it = tcpSockets.begin(); it != tcpSockets.end(); ++it) {
					if(it->isSetState(kSockState_WaitConnect)) {
						// 接続待ちソケット
						selector.addSocket(it->socket(), MySock::kSelectWrite | MySock::kSelectExcept);
					} else if(it->isSetState(kSockState_Connecting)) {
						// 接続済みソケット
						int selectflg = MySock::kSelectExcept;	// exceptは常にセット
						if(!it->isSetState(kSockState_Closing)) {
							// 接続中（受信可能、送信可能の両方を確認する）
							if(!it->isSetState(kSockState_Readable)) {
								selectflg |= MySock::kSelectRead;
							}
							if(!it->isSetState(kSockState_Writeable)) {
								selectflg |= MySock::kSelectWrite;
							}
						} else {
							// クローズ処理中（書込み可能かどうかだけ確認する）
							if(!it->isSetState(kSockState_Writeable)) {
								selectflg |= MySock::kSelectWrite;
							}
						}
						selector.addSocket(it->socket(), selectflg);
					} else {
						// 接続待ちでも接続済みでもなければ無視
					}
				}
				isNeedUpdateSelectSocket = false;
			}

			// select
			MySock::SelectResults selectResult = selector.select();

			{	// except
				MySock::SOCKET_LIST exceptSockets = selectResult[MySock::kSelectExcept];
				for(MySock::SOCKET_LIST::iterator it = exceptSockets.begin(); it != exceptSockets.end(); ++it) {
					for(StatefulTCPSocketList::iterator itTcp = tcpSockets.begin(); itTcp != tcpSockets.end(); ++itTcp) {
						CTCPSocketStateful& tcpSocket = (*itTcp);
						if(tcpSocket.socket() == (*it)) {
							isNeedUpdateSelectSocket = true;
							if(itTcp->isSetState(kSockState_WaitConnect)) {
								// 接続待ち 接続エラー
								std::wcout << _T("connect error") << std::endl;
							} else {
								// その他のエラー
								std::wcout << _T("except socket") << std::endl;
							}
							tcpSocket.close();
							tcpSockets.erase(itTcp);
							break;
						}
					}
				}
			}
			{	// write
				MySock::SOCKET_LIST writeSockets = selectResult[MySock::kSelectWrite];
				for(MySock::SOCKET_LIST::iterator it = writeSockets.begin(); it != writeSockets.end(); ++it) {
					for(StatefulTCPSocketList::iterator itTcp = tcpSockets.begin(); itTcp != tcpSockets.end(); ++itTcp) {
						CTCPSocketStateful& tcpSocket = (*itTcp);
						if(tcpSocket.socket() == (*it)) {
							isNeedUpdateSelectSocket = true;
							if(itTcp->isSetState(kSockState_WaitConnect)) {
								// 接続待ち→接続中状態へ
								itTcp->resetState(kSockState_WaitConnect);
								itTcp->setState(kSockState_Connecting);
								std::wcout << _T("connect socket ") << MySock::addressToString(&tcpSocket.getSockAddr().addr) <<
									_T(" to ") << MySock::addressToString(&tcpSocket.getPeerAddr().addr) << std::endl;
							} else if(itTcp->isSetState(kSockState_Connecting)) {
								// 接続中 送信可能フラグON
								itTcp->setState(kSockState_Writeable);
							} else {
								// その他
								std::wcout << _T("write invalid socket") << std::endl;
							}
							break;
						}
					}
				}
			}
			{	// read
				MySock::SOCKET_LIST readSockets = selectResult[MySock::kSelectWrite];
				for(MySock::SOCKET_LIST::iterator it = readSockets.begin(); it != readSockets.end(); ++it) {
					for(StatefulTCPSocketList::iterator itTcp = tcpSockets.begin(); itTcp != tcpSockets.end(); ++itTcp) {
						CTCPSocketStateful& tcpSocket = (*itTcp);
						if(tcpSocket.socket() == (*it)) {
							isNeedUpdateSelectSocket = true;
							if(itTcp->isSetState(kSockState_Connecting)) {
								// 接続中 受信可能フラグON
								itTcp->setState(kSockState_Readable);
							} else {
								// その他
								std::wcout << _T("read invalid socket") << std::endl;
							}
							break;
						}
					}
				}
			}

			{	// 送信可能、受信可能ソケットの処理
				for(StatefulTCPSocketList::iterator it = tcpSockets.begin(); it != tcpSockets.end(); ++it) {
					CTCPSocketStateful& tcpSocket = (*it);
					if(tcpSocket.isSetState(kSockState_Readable)) {
						// 受信可能
						MyLib::Data::BinaryData recvData = tcpSocket.recv();
						if(recvData.size() > 0) {
							// データ受信
							std::wcout << _T("recv data size=") << recvData.size() << std::endl <<
								_T(" my=") << MySock::addressToString(&tcpSocket.getSockAddr().addr) << std::endl <<
								_T(" peer=") << MySock::addressToString(&tcpSocket.getPeerAddr().addr) << std::endl <<
								MyLib::String::toHexStr(&recvData[0], recvData.size()) << std::endl;
						} else {
							// 接続先送信不可通知の受信（クローズ状態へ）
							tcpSocket.setState(kSockState_Closing);
						}

						tcpSocket.resetState(kSockState_Readable);
					}
					if(tcpSocket.isSetState(kSockState_Writeable)) {
						// 送信可能
						if(tcpSocket.canSend()) {
							// 送信必要
							tcpSocket.send(MyLib::Data::randomData(128));
							tcpSocket.resetState(kSockState_Writeable);
							tcpSocket.setSendTickCount(::GetTickCount());
						}
						if(!tcpSocket.isSetState(kSockState_Closing)) {
							// クローズ状態
							tcpSocket.shutdown(SD_SEND);
							tcpSocket.close();
							tcpSocket.setState(kSockState_Closed);
						}
					}
				}
				// クローズしたソケットをリストから削除
				std::remove_if(tcpSockets.begin(), tcpSockets.end(), IsClosedSocket());
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

		// ソケットの破棄
		for(StatefulTCPSocketList::iterator it = tcpSockets.begin(); it != tcpSockets.end(); ++it) {
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
