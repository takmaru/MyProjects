/*******************************************************************************
* ファイル名 : Singleton.h
* 名前空間   : Sync
* クラス     : CCriticalSection
* クラス     : CCriticalSectionLocker
*******************************************************************************/
#pragma once

/*******************************************************************************
*	インクルード
*******************************************************************************/
#include <Windows.h>

namespace MyLib {
namespace Sync {

/*******************************************************************************
*	CCriticalSectionクラス
*		CRITICAL_SECTIONのラッパークラス
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
*	CCriticalSectionLockerクラス
*		CCriticalSectionのヘルパクラス
*		コンストラクタの引数としてCCriticalSectionを渡すと、
*		自動でロックする。
*		デストラクタでロックを解除する。
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
