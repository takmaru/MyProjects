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

void socket_select(MySock::CSocketBase* sock, unsigned int timeout) {
	MySock::CSocketSelector selector;
	selector.addSocket(sock);
	MySock::SelectResults result = selector.select(timeout);
	std::tcout << _T("select result");
	for(MySock::SelectResults::iterator it = result.begin(); it != result.end(); ++it) {
		switch(it->first) {
		case MySock::kResultConnectSuccess:	std::tcout << _T(" [connect success]");	break;
		case MySock::kResultConnectFailed:	std::tcout << _T(" [connect error]");	break;
		case MySock::kResultAcceptable:		std::tcout << _T(" [acceptable]");		break;
		case MySock::kResultReadable:		std::tcout << _T(" [readable]");		break;
		case MySock::kResultWriteable:		std::tcout << _T(" [writeable]");		break;
		case MySock::kResultExcept:			std::tcout << _T(" [except]");			break;
		}
	}
	std::tcout << std::endl;
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

		// TCPサーバー select
		std::tcout << _T("TCPサーバーのイベント待ち クライアントconnect前") << std::endl;
		socket_select(&serverSocket, 0);

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

		// TCPサーバー select
		std::tcout << _T("TCPサーバーのイベント待ち クライアントconnect後") << std::endl;
		socket_select(&serverSocket, 0);

		// clientソケット select
		std::tcout << _T("clientソケットのイベント待ち　サーバーaccept前") << std::endl;
		socket_select(&clientSocket, 0);

		// TCPサーバー accept
		MySock::CTCPSocket acceptSocket = serverSocket.accept();

		// acceptソケット select
		std::tcout << _T("acceptソケットのイベント待ち サーバーaccept後") << std::endl;
		socket_select(&acceptSocket, 0);
		// clientソケット select
		std::tcout << _T("clientソケットのイベント待ち　サーバーaccept後") << std::endl;
		socket_select(&clientSocket, 0);

		// acceptソケット send
		acceptSocket.send(MyLib::Data::randomData(256));
		// clientソケット send
		clientSocket.send(MyLib::Data::randomData(128));

		// acceptソケット select
		std::tcout << _T("acceptソケットのイベント待ち send後") << std::endl;
		socket_select(&acceptSocket, 0);
		// clientソケット select
		std::tcout << _T("clientソケットのイベント待ち　send後") << std::endl;
		socket_select(&clientSocket, 0);

		// acceptソケット recv
		std::tcout << _T("acceptソケット recv=") << acceptSocket.recv().size() << _T("byte") << std::endl;
		// clientソケット recv
		std::tcout << _T("clientソケット recv=") << clientSocket.recv().size() << _T("byte") << std::endl;

		// acceptソケット select
		std::tcout << _T("acceptソケットのイベント待ち recv後") << std::endl;
		socket_select(&acceptSocket, 0);
		// clientソケット select
		std::tcout << _T("clientソケットのイベント待ち　recv後") << std::endl;
		socket_select(&clientSocket, 0);

		// GracefulClose from clientソケット
		clientSocket.shutdown(SD_SEND);
// clientソケット close
//clientSocket.close();
		// acceptソケット select
		std::tcout << _T("acceptソケットのイベント待ち clientソケットのshutdown後") << std::endl;
		socket_select(&acceptSocket, 0);
		// acceptソケット recv
		std::tcout << _T("acceptソケット recv=") << acceptSocket.recv().size() << _T("byte") << std::endl;
// acceptソケット recv
//std::tcout << _T("acceptソケット recv=") << acceptSocket.recv().size() << _T("byte") << std::endl;
		// acceptソケット send & shutdown
		acceptSocket.send(MyLib::Data::randomData(512));
		socket_select(&acceptSocket, 0);
		acceptSocket.send(MyLib::Data::randomData(64));
		socket_select(&acceptSocket, 0);
		// acceptソケット shutdown & close（GracefulClose中、両方のshutdown後、相手のソケットとこちらのソケットは閉じられる）
		acceptSocket.shutdown(SD_SEND);
		acceptSocket.close();
		// clientソケット select & recv
		std::tcout << _T("clientソケットのイベント待ち＆recv　acceptソケットのsend後") << std::endl;
		int clientRecvSize = 0;
		do {
			socket_select(&clientSocket, 0);
			clientRecvSize = clientSocket.recv().size();
			std::tcout << _T("clientソケット recv=") << clientRecvSize << _T("byte") << std::endl;
		} while(clientRecvSize != 0);
		socket_select(&clientSocket, 0);
		std::tcout << _T("clientソケット recv=") << clientSocket.recv().size() << _T("byte") << std::endl;
		// clientソケット close
		clientSocket.close();

		// serverソケット shutdownはエラー
		//serverSocket.shutdown(SD_RECEIVE);
		// serverソケット close
		serverSocket.close();

	} catch(MySock::CMySockException& e) {
		std::cout << e.what() << std::endl;
	}

	// WinSock 終了処理
	if(::WSACleanup() != 0) {
		std::tcout << _T("WSACleanup error=") << ::WSAGetLastError() << std::endl;
	}

	return 0;
}

