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

		// �T�[�o�[select�O �� TCP�N���C�A���g select
		std::tcout << _T("TCP�N���C�A���g�̃C�x���g�҂��@�T�[�o�[select�O") << std::endl;
		socket_select(clientSocket.socket(), INFINITE);

		// TCP�T�[�o�[ select
		std::tcout << _T("TCP�T�[�o�[�̃C�x���g�҂�") << std::endl;
		socket_select(serverSocket.socket(), INFINITE);

		// �T�[�o�[accept�O �� TCP�N���C�A���g select
		std::tcout << _T("TCP�N���C�A���g�̃C�x���g�҂��@�T�[�o�[accept�O") << std::endl;
		socket_select(clientSocket.socket(), INFINITE);

		clientSocket.send(MyLib::Data::randomData(128));

		// TCP�T�[�o�[ select
		std::tcout << _T("TCP�T�[�o�[�̃C�x���g�҂�") << std::endl;
		socket_select(serverSocket.socket(), INFINITE);

		// TCP�N���C�A���g select
		std::tcout << _T("TCP�N���C�A���g�̃C�x���g�҂��@�R���") << std::endl;
		socket_select(clientSocket.socket(), INFINITE);

	} catch(MySock::CMySockException& e) {
		std::cout << e.what() << std::endl;
	}

	// WinSock �I������
	if(::WSACleanup() != 0) {
		std::tcout << _T("WSACleanup error=") << ::WSAGetLastError() << std::endl;
	}

	return 0;
}

