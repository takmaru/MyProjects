// TCPSampleForResearch.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
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

	// ���P�[���ݒ�
	std::locale::global(std::locale("", std::locale::ctype));

	// WinSock ������
	WSADATA wsa = {0};
	int ret = ::WSAStartup(MAKEWORD(2, 2), &wsa);
	if(ret != 0) {
		std::tcout << _T("WSAStartup error=") << ret << std::endl;
		return -1;
	}

	try {

		// �T�[�o�[�\�P�b�g�쐬���҂���
		MySock::CTCPSocket serverSocket;
		{
			// �A�h���X���擾
			MySock::CAddrInfo ai(MySock::getAddrInfoTCP(NULL, 60000, AI_PASSIVE, AF_INET)[0]);
			// �T�[�o�[�\�P�b�g�쐬���҂���
			serverSocket.create(ai.family());
			serverSocket.bind(ai.sockaddr());
			serverSocket.listen();
		}

		// TCP�T�[�o�[ select
		std::tcout << _T("TCP�T�[�o�[�̃C�x���g�҂� �N���C�A���gconnect�O") << std::endl;
		socket_select(&serverSocket, 0);

		// TCP LISTENING �\�P�b�g�ɑ΂���UDP���M���Ă��A�v���g�R�����Ⴄ�̂Ŕ������Ȃ�
		/*
		std::tcout << _T("TCP�T�[�o�[��UDP���M") << std::endl;
		MySock::CUDPSocket udpSocket;
		{
			// �A�h���X���擾
			MySock::CAddrInfo ai(MySock::getAddrInfoUDP("localhost", 60000, 0, AF_INET)[0]);
			// �T�[�o�[�\�P�b�g�쐬���҂���
			udpSocket.create(ai.family());
			udpSocket.sendTo(ai.sockaddr(), MyLib::Data::randomData(128));
		}
		*/

		// TCP �N���C�A���g�̍쐬���ڑ�
		MySock::CTCPSocket clientSocket;
		{
			// �A�h���X���擾
			MySock::CAddrInfo ai(MySock::getAddrInfoTCP("localhost", 60000, AI_PASSIVE, AF_INET)[0]);
			// �T�[�o�[�\�P�b�g�쐬���҂���
			clientSocket.create(ai.family());
			clientSocket.setBlockingMode(false);
			clientSocket.connect(ai.sockaddr());
			/*	���s���ȃz�X�g�ւ̐ڑ����A��莞�Ԍo�ߌ��except�B
				�@���̊ԂɃ\�P�b�g�������N���[�Y����ƁAexcept�B
				���|�[�g���s���ȏꍇ�A��except�B	*/
		}

		// TCP�T�[�o�[ select
		std::tcout << _T("TCP�T�[�o�[�̃C�x���g�҂� �N���C�A���gconnect��") << std::endl;
		socket_select(&serverSocket, 0);

		// client�\�P�b�g select
		std::tcout << _T("client�\�P�b�g�̃C�x���g�҂��@�T�[�o�[accept�O") << std::endl;
		socket_select(&clientSocket, 0);

		// TCP�T�[�o�[ accept
		MySock::CTCPSocket acceptSocket = serverSocket.accept();

		// accept�\�P�b�g select
		std::tcout << _T("accept�\�P�b�g�̃C�x���g�҂� �T�[�o�[accept��") << std::endl;
		socket_select(&acceptSocket, 0);
		// client�\�P�b�g select
		std::tcout << _T("client�\�P�b�g�̃C�x���g�҂��@�T�[�o�[accept��") << std::endl;
		socket_select(&clientSocket, 0);

		// accept�\�P�b�g send
		acceptSocket.send(MyLib::Data::randomData(256));
		// client�\�P�b�g send
		clientSocket.send(MyLib::Data::randomData(128));

		// accept�\�P�b�g select
		std::tcout << _T("accept�\�P�b�g�̃C�x���g�҂� send��") << std::endl;
		socket_select(&acceptSocket, 0);
		// client�\�P�b�g select
		std::tcout << _T("client�\�P�b�g�̃C�x���g�҂��@send��") << std::endl;
		socket_select(&clientSocket, 0);

		// accept�\�P�b�g recv
		std::tcout << _T("accept�\�P�b�g recv=") << acceptSocket.recv().size() << _T("byte") << std::endl;
		// client�\�P�b�g recv
		std::tcout << _T("client�\�P�b�g recv=") << clientSocket.recv().size() << _T("byte") << std::endl;

		// accept�\�P�b�g select
		std::tcout << _T("accept�\�P�b�g�̃C�x���g�҂� recv��") << std::endl;
		socket_select(&acceptSocket, 0);
		// client�\�P�b�g select
		std::tcout << _T("client�\�P�b�g�̃C�x���g�҂��@recv��") << std::endl;
		socket_select(&clientSocket, 0);

		// GracefulClose from client�\�P�b�g
		clientSocket.shutdown(SD_SEND);
// client�\�P�b�g close
//clientSocket.close();
		// accept�\�P�b�g select
		std::tcout << _T("accept�\�P�b�g�̃C�x���g�҂� client�\�P�b�g��shutdown��") << std::endl;
		socket_select(&acceptSocket, 0);
		// accept�\�P�b�g recv
		std::tcout << _T("accept�\�P�b�g recv=") << acceptSocket.recv().size() << _T("byte") << std::endl;
// accept�\�P�b�g recv
//std::tcout << _T("accept�\�P�b�g recv=") << acceptSocket.recv().size() << _T("byte") << std::endl;
		// accept�\�P�b�g send & shutdown
		acceptSocket.send(MyLib::Data::randomData(512));
		socket_select(&acceptSocket, 0);
		acceptSocket.send(MyLib::Data::randomData(64));
		socket_select(&acceptSocket, 0);
		// accept�\�P�b�g shutdown & close�iGracefulClose���A������shutdown��A����̃\�P�b�g�Ƃ�����̃\�P�b�g�͕�����j
		acceptSocket.shutdown(SD_SEND);
		acceptSocket.close();
		// client�\�P�b�g select & recv
		std::tcout << _T("client�\�P�b�g�̃C�x���g�҂���recv�@accept�\�P�b�g��send��") << std::endl;
		int clientRecvSize = 0;
		do {
			socket_select(&clientSocket, 0);
			clientRecvSize = clientSocket.recv().size();
			std::tcout << _T("client�\�P�b�g recv=") << clientRecvSize << _T("byte") << std::endl;
		} while(clientRecvSize != 0);
		socket_select(&clientSocket, 0);
		std::tcout << _T("client�\�P�b�g recv=") << clientSocket.recv().size() << _T("byte") << std::endl;
		// client�\�P�b�g close
		clientSocket.close();

		// server�\�P�b�g shutdown�̓G���[
		//serverSocket.shutdown(SD_RECEIVE);
		// server�\�P�b�g close
		serverSocket.close();

	} catch(MySock::CMySockException& e) {
		std::cout << e.what() << std::endl;
	}

	// WinSock �I������
	if(::WSACleanup() != 0) {
		std::tcout << _T("WSACleanup error=") << ::WSAGetLastError() << std::endl;
	}

	return 0;
}

