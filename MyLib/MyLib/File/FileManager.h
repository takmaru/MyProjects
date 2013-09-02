#pragma once

#include <Windows.h>
#include <tchar.h>
#include "../tstring/tstring.h"

namespace MyLib {

namespace File {

class CFileManager {
public:
	static const int OM_Read		= 0x00000001;
	static const int OM_Write		= 0x00000002;
	static const int OM_ReadWrite	= (OM_Read | OM_Write);
public:
	CFileManager();
	explicit CFileManager(LPCTSTR strFileName);
	explicit CFileManager(unsigned int nOpenMode);
	CFileManager(LPCTSTR strFileName, unsigned int nOpenMode);
	virtual ~CFileManager();

public:
	void Open(){Open(m_FileName, m_OpenMode);};
	void Open(LPCTSTR strFileName){Open(strFileName, m_OpenMode);};
	void Open(unsigned int nOpenMode){Open(m_FileName, nOpenMode);};
	void Open(const std::tstring& strFileName, unsigned int nOpenMode){Open(strFileName.c_str(), nOpenMode);};
	void Open(LPCTSTR strFileName, unsigned int nOpenMode);
	void Close();

	void SetFileName(LPCTSTR strFileName);
	void SetOpenMode(int nOpenMode);

	bool IsOpen(){return (m_File != INVALID_HANDLE_VALUE);};
	HANDLE GetHandle(){return m_File;};
	std::tstring GetFileName(){return m_FileName;};
private:
	HANDLE m_File;
	std::tstring m_FileName;
	unsigned int m_OpenMode;
};

}
}
// ---------- End Of File --------------------------------------------------- //
