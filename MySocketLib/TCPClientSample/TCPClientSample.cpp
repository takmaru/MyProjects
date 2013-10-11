// TCPServerSample.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

#include <vector>

#include <MyLib/tstring/tstring.h>
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

//	typedef std::pair<MySock::CTCPSocket, int> SocketStatePair;
	class CTCPSocketStateful : public MySock::CTCPSocket {
	public:
		CTCPSocketStateful() : MySock::CTCPSocket(),
			m_state(0), m_lastDataSendTime() {
			m_lastDataSendTime.dwHighDateTime = 0;
			m_lastDataSendTime.dwLowDateTime = 0;
		}
		CTCPSocketStateful(const CTCPSocketStateful& obj) : MySock::CTCPSocket(obj),
			m_state(0), m_lastDataSendTime() {
			m_lastDataSendTime.dwHighDateTime = 0;
			m_lastDataSendTime.dwLowDateTime = 0;

		}
		CTCPSocketStateful(SOCKET sock, int family) : MySock::CTCPSocket(sock, family),
			m_state(0), m_lastDataSendTime() {
			m_lastDataSendTime.dwHighDateTime = 0;
			m_lastDataSendTime.dwLowDateTime = 0;

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
		bool isSetState(SocketState state) {
			return ((m_state & state) != 0);
		}
		SocketState state() const {
			return m_state;
		}
	private:
		SocketState m_state;
		FILETIME m_lastDataSendTime;
	};
	typedef std::vector<CTCPSocketStateful> StatefulTCPSocketList;
};


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
		MySock::AddrInfoList addrInfos = MySock::getAddrInfoTCP("S59009717", 60000, 0, AF_UNSPEC);

		// �\�P�b�g���X�g
		StatefulTCPSocketList tcpSockets;

		// TCP�N���C�A���g�\�P�b�g�쐬
		for(MySock::AddrInfoList::iterator it = addrInfos.begin(); it != addrInfos.end(); ++it) {
			CTCPSocketStateful sock;
			// TCP�N���C�A���g�\�P�b�g�쐬
			sock.create(it->family());
			sock.setBlockingMode(false);
			sock.connect(it->sockaddr());
			sock.setState(kSockState_WaitConnect);
			tcpSockets.push_back(sock);
		}

		// select�I�u�W�F�N�g
		MySock::CSocketSelector selector;
		// �ڑ��҂����[�v
		bool isNeedUpdateSelectSocket = true;
		while(1) {
			if(isNeedUpdateSelectSocket) {
				// select�I�u�W�F�N�g�����\�P�b�g�����N���A
				selector.clearSockets();
				for(StatefulTCPSocketList::iterator it = tcpSockets.begin(); it != tcpSockets.end(); ++it) {
					if(it->isSetState(kSockState_WaitConnect)) {
						// �ڑ��҂��\�P�b�g
						selector.addSocket(it->socket(), MySock::kSelectWrite | MySock::kSelectExcept);
					} else if(it->isSetState(kSockState_Connecting)) {
						// �ڑ��ς݃\�P�b�g
						int selectflg = MySock::kSelectExcept;	// except�͏�ɃZ�b�g
						if(!it->isSetState(kSockState_Readable)) {
							selectflg |= MySock::kSelectRead;
						}
						if(!it->isSetState(kSockState_Writeable)) {
							selectflg |= MySock::kSelectWrite;
						}
						selector.addSocket(it->socket(), selectflg);
					} else {
						// �ڑ��҂��ł��ڑ��ς݂ł��Ȃ���Ζ���
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
								// �ڑ��҂� �ڑ��G���[
								std::wcout << _T("connect error") << std::endl;
							} else {
								// ���̑��̃G���[
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
								// �ڑ��҂����ڑ�����Ԃ�
								itTcp->resetState(kSockState_WaitConnect);
								itTcp->setState(kSockState_Connecting);
								std::wcout << _T("connect socket ") << MySock::addressToString(&tcpSocket.getSockAddr().addr) <<
									_T(" to ") << MySock::addressToString(&tcpSocket.getPeerAddr().addr) << std::endl;
							} else if(itTcp->isSetState(kSockState_Connecting)) {
								// �ڑ��� ���M�\�t���OON
								itTcp->setState(kSockState_Writeable);
							} else {
								// ���̑�
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
								// �ڑ��� ��M�\�t���OON
								itTcp->setState(kSockState_Readable);
							} else {
								// ���̑�
								std::wcout << _T("read invalid socket") << std::endl;
							}
							break;
						}
					}
				}
			}

			{	// ���M�\�A��M�\�\�P�b�g�̏���
				for(StatefulTCPSocketList::iterator it = tcpSockets.begin(); it != tcpSockets.end(); ++it) {
					CTCPSocketStateful& tcpSocket = (*it);
					if(tcpSocket.isSetState(kSockState_Readable)) {
						// ��M���� or �N���[�Y����

						tcpSocket.resetState(kSockState_Readable);
					}
					if(tcpSocket.isSetState(kSockState_Writeable)) {
						// ���M����

						tcpSocket.resetState(kSockState_Writeable);
					}
				}
			}

			// �I���m�F
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

		// �\�P�b�g�̔j��
		for(StatefulTCPSocketList::iterator it = tcpSockets.begin(); it != tcpSockets.end(); ++it) {
			it->close();
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
