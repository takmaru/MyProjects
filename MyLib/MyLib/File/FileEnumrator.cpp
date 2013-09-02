#include "stdafx.h"
#include "FileEnumrator.h"

MyLib::File::CFileEnumrator::CFileEnumrator(const TCHAR* target) : m_list() {
	this->enumrate(target);
}
MyLib::File::CFileEnumrator::CFileEnumrator(const std::tstring& target) : m_list() {
	this->enumrate(target.c_str());
}

MyLib::File::CFileEnumrator::~CFileEnumrator() {
}

void MyLib::File::CFileEnumrator::enumrate(const TCHAR* target) {
	m_list.clear();
	// ファイル検索ループ
	WIN32_FIND_DATA finddata = {0};
	HANDLE find = ::FindFirstFile(target, &finddata);
	if(find != INVALID_HANDLE_VALUE) {
		do {
			// ".", ".." は無視
			if(	(_tcscmp(finddata.cFileName, _T(".")) != 0) &&
				(_tcscmp(finddata.cFileName, _T("..")) != 0)	) {
				// リストへ追加
					m_list.push_back(MyLib::File::CFileEnumrator::CFindData(finddata));
			}
		} while (	::FindNextFile(find, &finddata) ||
					(::GetLastError() == ERROR_MORE_DATA)	);
		::FindClose(find);
	}
}
