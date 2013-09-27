// UDPClientConnectSample.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
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

// �I���C�x���g
HANDLE g_exitEvent = NULL;
// ����R�[�h�ʒm�n���h���֐�
BOOL WINAPI HandlerRoutine(DWORD ctrlType);


int _tmain(int argc, _TCHAR* argv[]) {
	// ���P�[���ݒ�
	std::locale::global(std::locale("", std::locale::ctype));

	// �I���C�x���g�쐬
	g_exitEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if(g_exitEvent == NULL) {
		std::wcout << _T("CreateEvent error=") << ::GetLastError() << std::endl;
		return -1;
	}
	// ����R�[�h�ʒm�n���h���ǉ�
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
		// �\�P�b�g�쐬
		MySock::CUDPSocket socket_v4;
		socket_v4.create(AF_INET);
		MySock::CUDPSocket socket_v6;
		socket_v6.create(AF_INET6);

		// �T�[�o�[�̃A�h���X���擾
		MySock::AddrInfoList addrInfos = MySock::getAddrInfoUDP("S59009717", 60000, 0, AF_UNSPEC);
		int addr_id = 0;
		for(MySock::AddrInfoList::iterator it = addrInfos.begin(); it != addrInfos.end(); ++it) {
			std::tcout << addr_id << _T(" : ") << it->descpription() << std::endl;
			addr_id++;
		}

		// �f�[�^���M
		while(1) {
			int inputChar = _gettch();
			if(_istprint(inputChar)) {
				std::tcout << (TCHAR)inputChar << std::endl;
			}
			if( (inputChar == 3) ||
				(inputChar == _T('q')) ||
				(inputChar == _T('Q'))	) {
				std::tcout << _T("�I�����܂��B") << std::endl;
				break;
			} else if(_istdigit(inputChar)) {
				TCHAR inputStr[2] = {0};
				inputStr[0] = (TCHAR)inputChar;
				int inputValue = _ttoi(inputStr);
				if((unsigned int)inputValue < addrInfos.size()) {
					if(addrInfos[inputValue].family() == AF_INET) {
						socket_v4.sendTo(addrInfos[inputValue].sockaddr(), MyLib::Data::randomData(32));
						std::tcout << _T("���M���܂����B(IPv4) ") << MySock::addressToString(&socket_v4.getSockAddr().addr) << std::endl;
					} else if(addrInfos[inputValue].family() == AF_INET6) {
						socket_v6.sendTo(addrInfos[inputValue].sockaddr(), MyLib::Data::randomData(64));
						std::tcout << _T("���M���܂����B(IPv6) ") << MySock::addressToString(&socket_v6.getSockAddr().addr) << std::endl;
					} else {
						std::tcout << _T("����̃v���g�R�����s���ł��B family=") << addrInfos[inputValue].family() << std::endl;
					}
/*
					{
						MySock::CUDPClient udpClient;

						if(udpClient.sendTo("S59009717", 60000, MyLib::Data::randomData(128))) {
							std::wcout << _T("send success") << std::endl;
						} else {
							std::wcout << _T("send error") << std::endl;
							MySock::MYSOCKERRORS errors = udpClient.sock_errors();
							for(MySock::MYSOCKERRORS::iterator it = errors.begin(); it != errors.end(); ++it) {
								std::wcout << _T("type=") << (*it).type << _T(" error=") << (*it).error << std::endl;
							}
						}
					}
*/

				} else {
					std::tcout << _T("�s���ȓ��͒l�ł��B '0'�`'") << addrInfos.size() - 1 << _T("' ����͂��Ă��������B") << std::endl;
				}
			} else {
				std::tcout << _T("�s���ȓ��͒l�ł��B '0'�`'") << addrInfos.size() - 1 << _T("' ����͂��Ă��������B") << std::endl;
			}
		}
/*
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
*/

		// �\�P�b�gClose
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
