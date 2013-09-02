/*******************************************************************************
* �t�@�C���� : Singleton.h
* ���O���   : Sync
* �N���X     : CCriticalSection
* �N���X     : CCriticalSectionLocker
*******************************************************************************/
#pragma once

/*******************************************************************************
*	�C���N���[�h
*******************************************************************************/
#include <Windows.h>

namespace MyLib {
namespace Sync {

/*******************************************************************************
*	CCriticalSection�N���X
*		CRITICAL_SECTION�̃��b�p�[�N���X
*******************************************************************************/
class CCriticalSection {
public:
	CCriticalSection(){::InitializeCriticalSection(&m_cs);};
	virtual ~CCriticalSection(){::DeleteCriticalSection(&m_cs);};
public:
	void Lock(){::EnterCriticalSection(&m_cs);};
	void Unlock(){::LeaveCriticalSection(&m_cs);};
private:
	CRITICAL_SECTION m_cs;
};

/*******************************************************************************
*	CCriticalSectionLocker�N���X
*		CCriticalSection�̃w���p�N���X
*		�R���X�g���N�^�̈����Ƃ���CCriticalSection��n���ƁA
*		�����Ń��b�N����B
*		�f�X�g���N�^�Ń��b�N����������B
*******************************************************************************/
class CCriticalSectionLocker {
public:
	CCriticalSectionLocker(CCriticalSection& cs)
		: m_cs(cs){m_cs.Lock();};
	virtual ~CCriticalSectionLocker(){m_cs.Unlock();};
private:
	CCriticalSection& m_cs;
};

}
}
// ---------- End Of File --------------------------------------------------- //
