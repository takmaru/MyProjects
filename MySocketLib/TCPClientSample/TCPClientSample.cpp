// TCPServerSample.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
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
	// �I���C�x���g
	HANDLE g_exitEvent = NULL;
	// ����R�[�h�ʒm�n���h���֐�
	BOOL WINAPI HandlerRoutine(DWORD ctrlType);

	typedef std::map<MySock::CSocketBase*, DWORD, MySock::compareSocketBase> SocketSendTimeMap;
	typedef std::pair<MySock::CSocketBase*, DWORD> SocketSendTimePair;

	void OnReadable(MySock::CTCPSocket& sock);
	bool CheckExitEvent(bool& isExit);
};

int _tmain(int argc, _TCHAR* argv[]) {

	// �I���C�x���g�쐬
	g_exitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
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
		MySock::AddrInfoList addrInfos = MySock::getAddrInfoTCP("S59009717", 60000, 0, AF_UNSPEC);

		// �\�P�b�g���X�g
		MySock::SocketSet tcpSockets;
		// select�I�u�W�F�N�g
		MySock::CSocketSelector selector;

		// TCP�N���C�A���g�\�P�b�g�쐬
		for(MySock::AddrInfoList::iterator it = addrInfos.begin(); it != addrInfos.end(); ++it) {
			MySock::CTCPSocket* sock = new MySock::CTCPSocket();
			// TCP�N���C�A���g�\�P�b�g�쐬
			sock->create(it->family());
			sock->setBlockingMode(false);
			sock->connect(it->sockaddr());
			tcpSockets.insert(sock);
			// select�I�u�W�F�N�g�֒ǉ�
			selector.addSocket(sock);
		}

		SocketSendTimeMap socketSendtimes;
		bool isExit = false;
		while(CheckExitEvent(isExit)) {
			// �I���`�F�b�N�ŃG���[�Ȃ��i�G���[������΃��[�v�I���j

			if(isExit) {
				// �I������
				// �ڑ����̃\�P�b�g�S�ăV���b�g�_�E������
				for(MySock::SocketSet::iterator it = tcpSockets.begin(); it != tcpSockets.end(); ++it) {
					MySock::CTCPSocket* sock = dynamic_cast<MySock::CTCPSocket*>(*it);
					if(sock->state() == MySock::kSockState_Connecting) {
						sock->shutdown(SD_SEND);
					}
				}
			}

			// select
			MySock::SelectResults selectResult = selector.select();

			{	// �ڑ����s
				MySock::SocketSet sockets = selectResult[MySock::kResultConnectFailed];
				for(MySock::SocketSet::iterator it = sockets.begin(); it != sockets.end(); ++it) {
					std::tcout << _T("connect error sock=") << (*it)->socket() << _T(" err=") << (*it)->lastError() << std::endl;
				}
			}
			{	// �ڑ�����
				MySock::SocketSet sockets = selectResult[MySock::kResultConnectSuccess];
				for(MySock::SocketSet::iterator it = sockets.begin(); it != sockets.end(); ++it) {
					std::tcout << _T("connect success sock=") << (*it)->socket() << std::endl;
					socketSendtimes.insert(SocketSendTimePair((*it), 0));
				}
			}

			for(MySock::SocketSet::iterator it = tcpSockets.begin(); it != tcpSockets.end(); ++it) {
				MySock::CTCPSocket* sock = dynamic_cast<MySock::CTCPSocket*>(*it);
				if(sock->state() == MySock::kSockState_Connecting) {
					// ��ԁF�ڑ���
					if(sock->isIOState(MySock::kSocketIOState_Writeable)) {
						// Write�t���OON
						SocketSendTimeMap::iterator itFind = socketSendtimes.find(sock);
						if(itFind != socketSendtimes.end()) {
							DWORD now = ::GetTickCount();
							if(now - itFind->second > 1000) {
								// �Ō�ɑ��M���Ă�����莞�Ԉȏ�o�߂��Ă���΁A�f�[�^���M
								sock->send(MyLib::Data::randomData(128));
								itFind->second = now;
							}
						} else {
							// �\�P�b�g�ŏI�ڑ����ԃ}�b�v�Ɍ�����Ȃ������H
							std::tcout << _T("connected writeable socket not found ?") << std::endl;
						}
					}
					if(sock->isIOState(MySock::kSocketIOState_Readable)) {
						// Read�t���OON
						// �f�[�^��M����
						OnReadable(*sock);
					}
				} else if(sock->state() == MySock::kSockState_GracefulClosing) {
					// ��ԁF�N���[�Y������
					if(	sock->isIOState(MySock::kSocketIOState_RecvFin) &&
						!sock->isIOState(MySock::kSocketIOState_SendFin)	) {
						// Fin��M�AFin�����M
						// �V���b�g�_�E��
						sock->shutdown(SD_SEND);
					} else
					if(	!sock->isIOState(MySock::kSocketIOState_RecvFin) &&
						sock->isIOState(MySock::kSocketIOState_SendFin)	){
						// Fin���M�AFin����M
						if(sock->isIOState(MySock::kSocketIOState_Readable)) {
							// Read�t���OON
							// �f�[�^��M����
							OnReadable(*sock);
						}
					}

					if(sock->isIOState(MySock::kSocketIOState_FinBoth)) {
						// Fin�𑗐M���AFin����M���Ă���Ȃ�I������
						selector.removeSocket(sock);
						sock->close();
					}
				}
			}

			if(isExit) {
				// �I������
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
					// �S�Ẵ\�P�b�g���ڑ����s�i�쐬��j���ڑ����\�P�b�g�����ł���΃��[�v�I��
					break;
				}
			}
		}

		// �\�P�b�g�̔j��
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

namespace {

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

void OnReadable(MySock::CTCPSocket& sock) {
	bool isFinRecv = false;
	MyLib::Data::BinaryData recvData = sock.recv(&isFinRecv);
	if(!isFinRecv) {
		// �f�[�^��M
		std::wcout << _T("recv data size=") << recvData.size() << std::endl <<
			_T(" my=") << MySock::addressToString(&(sock.getSockAddr().addr)) << std::endl <<
			_T(" peer=") << MySock::addressToString(&(sock.getPeerAddr().addr)) << std::endl <<
			MyLib::String::toHexStr(&recvData[0], recvData.size()) << std::endl;
	} else {
		// Fin��M
		std::tcout << _T("Fin recv sock=") << sock.socket() << std::endl;
	}
}

bool CheckExitEvent(bool& isExit) {
	// �I���m�F
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