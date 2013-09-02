#include "stdafx.h"
#include "FileUtil.h"

#include "FileEnumrator.h"
#include "../Path/Path.h"

bool MyLib::File::copy(std::tstring src, std::tstring dst, bool isOverride) {

	if(!MyLib::Path::isExsist(src)) {
		return false;
	}

	bool result = false;
	if(!MyLib::Path::isDirectory(src)) {
		// �t�@�C���R�s�[
		if(!::CopyFile(src.c_str(), dst.c_str(), !isOverride)) {
			return false;
		}
	} else {
		// �R�s�[��f�B���N�g�����݃`�F�b�N
		if(!MyLib::Path::isExsist(dst)) {
			if(!::CreateDirectory(dst.c_str(), NULL)) {
				return false;
			}
		} else if(!isOverride) {
			// �㏑���s�Ȃ玸�s
			return false;
		}

		// �f�B���N�g���R�s�[
		WIN32_FIND_DATA fd = {0};
		HANDLE find = ::FindFirstFile(MyLib::Path::append(src, _T("*")).c_str(), &fd);
		if(find != INVALID_HANDLE_VALUE) {

			bool isError = false;
			do {
				// ".", ".." �͖���
				if(	(_tcscmp(fd.cFileName, _T(".")) != 0) &&
					(_tcscmp(fd.cFileName, _T("..")) != 0)	) {
					std::tstring findSrc = MyLib::Path::append(src, fd.cFileName);
					if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
						// �t�@�C���Ȃ�R�s�[
						if(!::CopyFile(findSrc.c_str(), MyLib::Path::append(dst, fd.cFileName).c_str(), !isOverride)) {
							isError = true;
							break;
						}
					} else {
						// �f�B���N�g���Ȃ�ċA
						if(!MyLib::File::copy(findSrc, MyLib::Path::append(dst, fd.cFileName), isOverride)) {
							isError = true;
							break;
						}
					}
				}
			} while(	::FindNextFile(find, &fd) ||
						(::GetLastError() == ERROR_MORE_DATA)	);
			::FindClose(find);

			result = !isError;
		}
	}
	return result;
}

bool MyLib::File::remove(std::tstring path) {

	if(!MyLib::Path::isExsist(path)) {
		return true;
	}

	bool result = false;
	if(!MyLib::Path::isDirectory(path)) {
		// �t�@�C�� �폜
		if(::DeleteFile(path.c_str())) {
			result = true;
		}
	} else {
		// �f�B���N�g��
		// �f�B���N�g�����t�@�C����
		MyLib::File::CFileEnumrator::FindList findfiles = MyLib::File::CFileEnumrator(MyLib::Path::append(path, _T("*"))).finddata_list();
		for(MyLib::File::CFileEnumrator::FindList::const_iterator it = findfiles.begin(); it != findfiles.end(); ++it) {
			// �ċA
			if(!MyLib::File::remove(MyLib::Path::append(path, (*it).filename()))) {
				break;
			}
		}
		// �f�B���N�g���폜
		if(::RemoveDirectory(path.c_str())) {
			result = true;
		}
	}
	return result;
}
