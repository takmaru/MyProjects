// TCPServerSample.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

#include <MyLib/tstring/tstring.h>
#include <MySocketLib/AddrInfo.h>
#include <MySocketLib/TCPSocket.h>
#include <MySocketLib/SocketSelector.h>
#include <MySocketLib/MySockTypedef.h>
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
	const SocketState kSockState_Listening		= 0x00000040;

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
		MySock::AddrInfoList addrInfos = MySock::getAddrInfoTCP(NULL, 60000, AI_PASSIVE, AF_UNSPEC);
		// �҂��󂯃\�P�b�g��select�I�u�W�F�N�g �쐬
		MySock::TCPSocketList listenSockets;
		MySock::CSocketSelector selector;
		for(MySock::AddrInfoList::iterator it = addrInfos.begin(); it != addrInfos.end(); ++it) {
			MySock::CTCPSocket sock;
			// �҂��󂯃\�P�b�g�쐬
			sock.create(it->family());
			sock.bind(it->sockaddr());
			sock.listen();
			std::wcout << MySock::addressToString(&sock.getSockAddr().addr) << std::endl;
			// select�I�u�W�F�N�g�֒ǉ�
			selector.addSocket(sock.socket(), MySock::kSelectRead);
			listenSockets.push_back(sock);
		}

		// �ڑ��҂��󂯃��[�v
		MySock::TCPSocketList acceptSockets;
		while(1) {
			// select
			MySock::SelectResults selectResult = selector.select();

			{	// read
				MySock::SOCKET_LIST readSockets = selectResult[MySock::kSelectRead];
				for(MySock::SOCKET_LIST::iterator it = readSockets.begin(); it != readSockets.end(); ++it) {
					for(MySock::TCPSocketList::iterator itTcp = listenSockets.begin(); itTcp != listenSockets.end(); ++itTcp) {
						if(itTcp->socket() == (*it)) {
							// accept
							MySock::MySockAddr sockaddr = {0};
							acceptSockets.push_back(itTcp->accept(&sockaddr));
							MySock::CTCPSocket& sock = acceptSockets.back();
							std::wcout << _T("accept socket ") << MySock::addressToString(&sock.getSockAddr().addr) <<
								_T(" from ") << MySock::addressToString(&sock.getPeerAddr().addr) <<
								_T(" accept ") << MySock::addressToString(&sockaddr.addr) << std::endl;
						}
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

		// �ڑ��ς݃\�P�b�g ��~
		for(MySock::TCPSocketList::iterator it = acceptSockets.begin(); it != acceptSockets.end(); ++it) {
			it->close();
		}
		// TCP�T�[�o�[ ��~
		for(MySock::TCPSocketList::iterator it = listenSockets.begin(); it != listenSockets.end(); ++it) {
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
