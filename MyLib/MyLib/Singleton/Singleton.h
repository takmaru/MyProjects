/*******************************************************************************
* ファイル名 : Singleton.h
* クラス     : Singleton
*******************************************************************************/
#pragma once

/*******************************************************************************
*	インクルード
*******************************************************************************/
#include "../Sync/CriticalSection.h"

namespace MyLib {
namespace Singleton {

/*******************************************************************************
*	Singletonクラス
*		任意のクラスをシングルトン化する。
*		但し、デフォルトコンストラクタが必要
*		マルチスレッド対応済
*******************************************************************************/
template<class T>
class Singleton {
private:
	static MyLib::Sync::CCriticalSection criticalSection;
	static T* singleton;
public:
	static T* GetInstance() {
		if(!singleton) {
			MyLib::Sync::CCriticalSectionLocker locker(criticalSection);
			if(!singleton) {
				static T instance;
				singleton = &instance;
			}
		}
		return singleton;
	};
};

template<class T> MyLib::Sync::CCriticalSection Singleton<T>::criticalSection;
template<class T> T* Singleton<T>::singleton = NULL;

}
}
// ---------- End Of File --------------------------------------------------- //
