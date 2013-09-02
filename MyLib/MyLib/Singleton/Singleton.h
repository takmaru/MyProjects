/*******************************************************************************
* �t�@�C���� : Singleton.h
* �N���X     : Singleton
*******************************************************************************/
#pragma once

/*******************************************************************************
*	�C���N���[�h
*******************************************************************************/
#include "../Sync/CriticalSection.h"

namespace MyLib {
namespace Singleton {

/*******************************************************************************
*	Singleton�N���X
*		�C�ӂ̃N���X���V���O���g��������B
*		�A���A�f�t�H���g�R���X�g���N�^���K�v
*		�}���`�X���b�h�Ή���
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
