// UDPClientConnectSample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <conio.h>

#include <MySocketLib/AddrInfo.h>
#include <MySocketLib/UDPSocket.h>
#include <MySocketLib/SocketAddr.h>
#include <MySocketLib/MySockException.h>
#include <MySocketLib/MySockUtil.h>
#include <MyLib/Data/BinaryData.h>
#include <MyLib/Data/DataUtil.h>
#include <MyLib/tstring/tstring.h>

// 終了イベント
HANDLE g_exitEvent = NULL;
// 制御コード通知ハンドラ関数
BOOL WINAPI HandlerRoutine(DWORD ctrlType);


int _tmain(int argc, _TCHAR* argv[]) {
	// ロケール設定
	std::locale::global(std::locale("", std::locale::ctype));

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

	WSADATA wsa = {0};
	int ret = ::WSAStartup(MAKEWORD(2, 2), &wsa);
	if(ret != 0) {
		std::wcout << _T("WSAStartup error=") << ret << std::endl;
		return -1;
	}

	try {
		// ソケット作成
		MySock::CUDPSocket socket_v4;
		socket_v4.create(AF_INET);
		MySock::CUDPSocket socket_v6;
		socket_v6.create(AF_INET6);
		bool isConnect_v4 = false;
		bool isConnect_v6 = false;

		// サーバーのアドレス情報取得
		MySock::AddrInfoList addrInfos = MySock::getAddrInfoUDP("S59009717", 60000, 0, AF_UNSPEC);
		int addr_id = 0;
		for(MySock::AddrInfoList::iterator it = addrInfos.begin(); it != addrInfos.end(); ++it) {
			std::tcout << addr_id << _T(" : ") << it->descpription() << std::endl;
			addr_id++;
		}

		// データ送信
		static const int kInitialMode = 0;
		static const int kConnectMode = 1;
		static const int kSendToMode = 2;
		static const int kSendMode = 3;
		int mode = kInitialMode;
		while(1) {
			// 表示
			if(mode == kConnectMode) {
				std::tcout << _T("connect: 0 - ") << addrInfos.size() - 1 << std::endl;
			} else if(mode == kSendToMode) {
				std::tcout << _T("sendTo: 0 - ") << addrInfos.size() - 1 << std::endl;
			} else if(mode == kSendMode) {
				std::tcout << _T("send: 4, 6") << std::endl;
			} else {
				std::tcout << _T("mode:c, t, s") << std::endl;
			}

			// 入力待ち
			int inputChar = _gettch();
			if(_istprint(inputChar)) {
				std::tcout << (TCHAR)inputChar << std::endl;
			}
			if( (inputChar == 3) ||	// Ctrl+C
				(inputChar == _T('q')) || (inputChar == _T('Q')) ) {
				std::tcout << _T("終了します。") << std::endl;
				break;
			};
			if(mode == kInitialMode) {
				if( (inputChar == _T('c')) || (inputChar == _T('C')) ) {
					std::tcout << _T("connect to ...") << std::endl;
					mode = kConnectMode;
				} else if( (inputChar == _T('t')) || (inputChar == _T('T')) ) {
					std::tcout << _T("send to ...") << std::endl;
					mode = kSendToMode;
				} else if( (inputChar == _T('s')) || (inputChar == _T('S')) ) {
					std::tcout << _T("send ...") << std::endl;
					mode = kSendMode;
				}
			} else {
				if(_istdigit(inputChar)) {
					if(mode == kConnectMode) {
						// connect
						TCHAR inputStr[2] = {0};
						inputStr[0] = (TCHAR)inputChar;
						int inputValue = _ttoi(inputStr);
						if((unsigned int)inputValue < addrInfos.size()) {
							MySock::CAddrInfo& addr = addrInfos[inputValue];
							if(addr.family() == AF_INET) {
								socket_v4.connect(addr.sockaddr());
								isConnect_v4 = true;
								std::tcout << _T("接続しました。(IPv4) ") << MySock::addressToString(&socket_v4.getSockAddr().addr) <<
									_T(" to ") << MySock::addressToString(&socket_v4.getPeerAddr().addr) << std::endl;
							} else if(addr.family() == AF_INET6) {
								socket_v6.connect(addr.sockaddr());
								isConnect_v6 = true;
								std::tcout << _T("接続しました。(IPv6) ") << MySock::addressToString(&socket_v6.getSockAddr().addr) <<
									_T(" to ") << MySock::addressToString(&socket_v6.getPeerAddr().addr) << std::endl;
							} else {
								std::tcout << _T("SendToMode 宛先のプロトコルが不明です。 family=") << addr.family() << std::endl;
							}
						}

					} else if(mode == kSendToMode) {
						// send To
						TCHAR inputStr[2] = {0};
						inputStr[0] = (TCHAR)inputChar;
						int inputValue = _ttoi(inputStr);
						if((unsigned int)inputValue < addrInfos.size()) {
							MySock::CAddrInfo& addr = addrInfos[inputValue];
							if(addr.family() == AF_INET) {
								socket_v4.sendTo(addr.sockaddr(), MyLib::Data::randomData(32));
								std::tcout << _T("送信しました。(IPv4) ") << MySock::addressToString(&socket_v4.getSockAddr().addr) <<
									_T(" to ") << MySock::addressToString(&addr.sockaddr().addr) << std::endl;
							} else if(addr.family() == AF_INET6) {
								socket_v6.sendTo(addr.sockaddr(), MyLib::Data::randomData(64));
								std::tcout << _T("送信しました。(IPv6) ") << MySock::addressToString(&socket_v6.getSockAddr().addr) <<
									_T(" to ") << MySock::addressToString(&addr.sockaddr().addr) << std::endl;
							} else {
								std::tcout << _T("SendToMode 宛先のプロトコルが不明です。 family=") << addr.family() << std::endl;
							}
						}
						
					} else if(mode == kSendMode) {
						// send
						TCHAR inputStr[2] = {0};
						inputStr[0] = (TCHAR)inputChar;
						int inputValue = _ttoi(inputStr);
						if(inputValue == 4) {
							if(isConnect_v4) {
								socket_v4.send(MyLib::Data::randomData(32));
								std::tcout << _T("送信しました。(IPv4) ") << MySock::addressToString(&socket_v4.getSockAddr().addr) <<
									_T(" to ") << MySock::addressToString(&socket_v4.getPeerAddr().addr) << std::endl;
							} else {
								std::tcout << _T("未接続です。(IPv4) ") << std::endl;
							}
						} else if(inputValue == 6) {
							if(isConnect_v6) {
								socket_v6.send(MyLib::Data::randomData(64));
								std::tcout << _T("送信しました。(IPv6) ") << MySock::addressToString(&socket_v6.getSockAddr().addr) <<
									_T(" to ") << MySock::addressToString(&socket_v6.getPeerAddr().addr) << std::endl;
							} else {
								std::tcout << _T("未接続です。(IPv6) ") << std::endl;
							}
						}
					}

					mode = kInitialMode;
				}
			}
		}

		// ソケットClose
		socket_v4.close();
		socket_v6.close();
	} catch(MySock::CMySockException& e) {
		std::cout << e.what() << std::endl;
	}

	if(::WSACleanup() != 0) {
		std::wcout << _T("WSACleanup error=") << ::WSAGetLastError() << std::endl;
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
