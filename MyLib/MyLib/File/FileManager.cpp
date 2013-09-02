#include "stdafx.h"
#include "FileManager.h"

#include "../Exception/MyException.h"

using namespace MyLib::File;

CFileManager::CFileManager():
	m_File(INVALID_HANDLE_VALUE), m_FileName(), m_OpenMode(OM_ReadWrite){}
CFileManager::CFileManager(LPCTSTR strFileName):
	m_File(INVALID_HANDLE_VALUE), m_FileName(strFileName), m_OpenMode(OM_ReadWrite){}
CFileManager::CFileManager(unsigned int nOpenMode):
	m_File(INVALID_HANDLE_VALUE), m_FileName(), m_OpenMode(nOpenMode){}
CFileManager::CFileManager(LPCTSTR strFileName, unsigned int nOpenMode):
	m_File(INVALID_HANDLE_VALUE), m_FileName(strFileName), m_OpenMode(nOpenMode){}
CFileManager::~CFileManager(){Close();}

void CFileManager::Open(LPCTSTR strFileName, unsigned int nOpenMode)
{
	if(IsOpen())	RAISE_MYEXCEPTION("File Open çœ");

	DWORD dwDesiredAccess = 0;
	if((nOpenMode & OM_Read) != 0)	dwDesiredAccess |= GENERIC_READ;
	if((nOpenMode & OM_Write) != 0)	dwDesiredAccess |= GENERIC_WRITE;
	DWORD dwShareMode = 0;
	if((nOpenMode & OM_Write) != 0)		dwShareMode = FILE_SHARE_READ;
	else if((nOpenMode & OM_Read) != 0)	dwShareMode = FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD dwCreateDisposition = 0;
	if((nOpenMode & OM_Write) != 0)		dwCreateDisposition = OPEN_ALWAYS;
	else if((nOpenMode & OM_Read) != 0)	dwCreateDisposition = OPEN_EXISTING;
	m_File = ::CreateFileW(strFileName, dwDesiredAccess, dwShareMode, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(m_File == INVALID_HANDLE_VALUE)	RAISE_MYEXCEPTION("File Open é∏îs");

	m_FileName = strFileName;
	m_OpenMode = nOpenMode;
}
void CFileManager::Close(){if(IsOpen())	::CloseHandle(m_File);}

void CFileManager::SetFileName(LPCTSTR strFileName)
{
	if(IsOpen())	RAISE_MYEXCEPTION("File Open çœ");
	m_FileName = strFileName;
}
void CFileManager::SetOpenMode(int nOpenMode)
{
	if(IsOpen())	RAISE_MYEXCEPTION("File Open çœ");
	m_OpenMode = nOpenMode;
}
// ---------- End Of File --------------------------------------------------- //
