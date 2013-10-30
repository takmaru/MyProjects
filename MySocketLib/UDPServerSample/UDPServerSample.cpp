// UDPServerSample.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

#include <winsock2.h>
#include <wincon.h>

#include <iostream>

#include <MySocketLib/AddrInfo.h>
#include <MySocketLib/UDPSocket.h>
#include <MySocketLib/SocketSelector.h>
#include <MySocketLib/MySockUtil.h>
#include <MySocketLib/MySockException.h>
#include <MyLib/String/StringUtil.h>
#include <MyLib/tstring/tstring.h>

// �I���C�x���g
HANDLE g_exitEvent = NULL;
// ����R�[�h�ʒm�n���h���֐�
BOOL WINAPI HandlerRoutine(DWORD ctrlType);

int _tmain(int argc, _TCHAR* argv[]) {

	// �I���C�x���g�쐬
	g_exitEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if(g_exitEvent == NULL) {
		std::tcout << _T("CreateEvent error=") << ::GetLastError() << std::endl;
		return -1;
	}
	// ����R�[�h�ʒm�n���h���ǉ�
	if(!::SetConsoleCtrlHandler(&HandlerRoutine, TRUE)) {
		std::tcout << _T("SetConsoleCtrlHandler(set) error=") << ::GetLastError() << std::endl;
	}

	// WinSock ������
	WSADATA wsa = {0};
	int ret = ::WSAStartup(MAKEWORD(2, 2), &wsa);
	if(ret != 0) {
		std::tcout << _T("WSAStartup error=") << ret << std::endl;
		return -1;
	}

	try {
		// �A�h���X���擾
		MySock::AddrInfoList addrInfos = MySock::getAddrInfoUDP(NULL, 60000, AI_PASSIVE, AF_UNSPEC);
		// �҂��󂯃\�P�b�g��select�I�u�W�F�N�g �쐬
		MySock::SocketSet udpSockets;
		MySock::CSocketSelector selector;
		for(MySock::AddrInfoList::iterator it = addrInfos.begin(); it != addrInfos.end(); ++it) {
			MySock::CUDPSocket* sock = new MySock::CUDPSocket();
			// �҂��󂯃\�P�b�g�쐬
			sock->create(it->family());
			sock->bind(it->sockaddr());
			std::wcout << MySock::addressToString(&(sock->getSockAddr().addr)) << std::endl;
			// select�I�u�W�F�N�g�֒ǉ�
			udpSockets.insert(sock);
			selector.addSocket(sock);
		}

		// �f�[�^��M���[�v
		while(1) {
			// select
			MySock::SelectResults selectResult = selector.select();
			
			{	// read
				MySock::SocketSet readSockets = selectResult[MySock::kResultReadable];
				for(MySock::SocketSet::iterator it = readSockets.begin(); it != readSockets.end(); ++it) {
					MySock::CUDPSocket* sock = dynamic_cast<MySock::CUDPSocket*>(*it);
					if(sock != NULL) {
						// ��M
						MyLib::Data::BinaryData recvData;
						MySock::MySockAddr sockaddr = {0};
						bool isSendError = false;
						sock->recv(recvData, &sockaddr, isSendError);
						if(!isSendError) {
							std::wcout << _T("!! arrived Data size=") << recvData.size() << std::endl <<
								("from=") << MySock::addressToString(&sockaddr.addr) << std::endl <<
								("to=") << MySock::addressToString(&sock->getSockAddr().addr) << std::endl <<
								MyLib::String::toHexStr(&recvData[0], recvData.size()) << std::endl;
							// ���̂܂ܕԐM
							sock->sendTo(sockaddr, recvData);
						} else {
							std::wcout << _T("!! last send data is Unreachabled") <<
								(" sock=") << MySock::addressToString(&sock->getSockAddr().addr) << std::endl;
						}
					} else {
						std::tcout << _T("socket protocol(") << (*it)->protocol() << _T(") isn't udp?") << std::endl;
					}
				}
			}

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

		// UDP�T�[�o�[ ��~
		for(MySock::SocketSet::iterator it = udpSockets.begin(); it != udpSockets.end(); ++it) {
			(*it)->close();
			delete (*it);
		}

	} catch(MySock::CMySockException& e) {
		std::cout << e.what() << std::endl;
	}

	// WinSock �I������
	if(::WSACleanup() != 0) {
		std::tcout << _T("WSACleanup error=") << ::WSAGetLastError() << std::endl;
	}

	// ����R�[�h�ʒm�n���h���폜
	if(!::SetConsoleCtrlHandler(&HandlerRoutine, FALSE)) {
		std::tcout << _T("SetConsoleCtrlHandler(del) error=") << ::GetLastError() << std::endl;
	}
	// �I���C�x���g�j��
	if(!::CloseHandle(g_exitEvent)) {
		std::tcout << _T("CloseHandle error=") << ::GetLastError() << std::endl;
	}

	return 0;
}

BOOL WINAPI HandlerRoutine(DWORD ctrlType) {
	std::wcout << _T("on ctrl=") << ctrlType << std::endl;

	BOOL res = FALSE;
	switch(ctrlType) {
	// Ctrl+C, Ctrl+Break�͏I������������ׂ�TRUE��Ԃ�
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
	// ��L�ȊO(Close, ���O�I�t, �V���b�g�_�E��)�͋����I��������
	default:
		res = FALSE;
		break;
	}
	return res;
}
