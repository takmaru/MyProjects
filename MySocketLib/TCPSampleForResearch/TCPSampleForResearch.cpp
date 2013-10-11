// TCPSampleForResearch.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <locale>

#include <MyLib/tstring/tstring.h>
#include <MyLib/Data/BinaryData.h>
#include <MyLib/Data/DataUtil.h>
#include <MySocketLib/MySockException.h>
#include <MySocketLib/TCPSocket.h>
#include <MySocketLib/UDPSocket.h>
#include <MySocketLib/SocketSelector.h>
#include <MySocketLib/AddrInfo.h>

void socket_select(SOCKET s, unsigned int timeout) {
	MySock::CSocketSelector selector;
	selector.addSocket(s, MySock::kSelectRead | MySock::kSelectExcept | MySock::kSelectWrite);
	MySock::SelectResults result = selector.select(timeout);
	std::tcout << _T("select result") <<
		_T(" read=") << result[MySock::kSelectRead].size() <<
		_T(" write=") << result[MySock::kSelectWrite].size() <<
		_T(" except=") << result[MySock::kSelectExcept].size() <<
		std::endl;
}

int _tmain(int argc, _TCHAR* argv[]) {

	// ロケール設定
	std::locale::global(std::locale("", std::locale::ctype));

	// WinSock 初期化
	WSADATA wsa = {0};
	int ret = ::WSAStartup(MAKEWORD(2, 2), &wsa);
	if(ret != 0) {
		std::tcout << _T("WSAStartup error=") << ret << std::endl;
		return -1;
	}

	try {

		// サーバーソケット作成＆待ち受け
		MySock::CTCPSocket serverSocket;
		{
			// アドレス情報取得
			MySock::CAddrInfo ai(MySock::getAddrInfoTCP(NULL, 60000, AI_PASSIVE, AF_INET)[0]);
			// サーバーソケット作成＆待ち受け
			serverSocket.create(ai.family());
			serverSocket.bind(ai.sockaddr());
			serverSocket.listen();
		}

		// TCP LISTENING ソケットに対してUDP送信しても、プロトコルが違うので反応しない
		/*
		std::tcout << _T("TCPサーバーへUDP送信") << std::endl;
		MySock::CUDPSocket udpSocket;
		{
			// アドレス情報取得
			MySock::CAddrInfo ai(MySock::getAddrInfoUDP("localhost", 60000, 0, AF_INET)[0]);
			// サーバーソケット作成＆待ち受け
			udpSocket.create(ai.family());
			udpSocket.sendTo(ai.sockaddr(), MyLib::Data::randomData(128));
		}
		*/

		// TCP クライアントの作成＆接続
		MySock::CTCPSocket clientSocket;
		{
			// アドレス情報取得
			MySock::CAddrInfo ai(MySock::getAddrInfoTCP("localhost", 60000, AI_PASSIVE, AF_INET)[0]);
			// サーバーソケット作成＆待ち受け
			clientSocket.create(ai.family());
			clientSocket.setBlockingMode(false);
			clientSocket.connect(ai.sockaddr());
			/*	※不正なホストへの接続時、一定時間経過後にexcept。
				　その間にソケットを強制クローズすると、except。
				※ポートが不正な場合、即except。	*/
		}

		// サーバーselect前 の TCPクライアント select
		std::tcout << _T("TCPクライアントのイベント待ち　サーバーselect前") << std::endl;
		socket_select(clientSocket.socket(), INFINITE);

		// TCPサーバー select
		std::tcout << _T("TCPサーバーのイベント待ち") << std::endl;
		socket_select(serverSocket.socket(), INFINITE);

		// サーバーaccept前 の TCPクライアント select
		std::tcout << _T("TCPクライアントのイベント待ち　サーバーaccept前") << std::endl;
		socket_select(clientSocket.socket(), INFINITE);

		clientSocket.send(MyLib::Data::randomData(128));

		// TCPサーバー select
		std::tcout << _T("TCPサーバーのイベント待ち") << std::endl;
		socket_select(serverSocket.socket(), INFINITE);

		// TCPクライアント select
		std::tcout << _T("TCPクライアントのイベント待ち　３回目") << std::endl;
		socket_select(clientSocket.socket(), INFINITE);

	} catch(MySock::CMySockException& e) {
		std::cout << e.what() << std::endl;
	}

	// WinSock 終了処理
	if(::WSACleanup() != 0) {
		std::tcout << _T("WSACleanup error=") << ::WSAGetLastError() << std::endl;
	}

	return 0;
}

