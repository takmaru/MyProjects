#pragma once

#include <Windows.h>
#include <tchar.h>
#include <list>
#include <string>
#include "../tstring/tstring.h"

namespace MyLib {
namespace File {

/*
 *	使用例：
 *		MyLib::File::CFileEnumrator::FindList findfiles = MyLib::File::CFileEnumrator(MyLib::Path::append(<フォルダパス>, _T("*"))).finddata_list();
 *		MyLib::File::CFileEnumrator::FileList findfiles = MyLib::File::CFileEnumrator(MyLib::Path::append(<フォルダパス>, _T("*"))).filelist();
*/
class CFileEnumrator {
public:
	class CFindData {
	public:
		explicit CFindData(const WIN32_FIND_DATA& findData):
			m_attribute(findData.dwFileAttributes),
			m_createTime(findData.ftCreationTime), m_lastAccessTime(findData.ftLastAccessTime), m_lastWriteTime(findData.ftLastWriteTime),
			m_filesize(((ULONGLONG)findData.nFileSizeHigh << 32) | (ULONGLONG)findData.nFileSizeLow),
			m_filename(findData.cFileName) {
		};
		~CFindData(){};
	public:
		bool isDirectory() const {
			return ((m_attribute & FILE_ATTRIBUTE_DIRECTORY) != 0);
		};
	public:
		DWORD attribute() const {
			return m_attribute;
		};
		FILETIME createTime() const {
			return m_createTime;
		};
		FILETIME lastAccessTime() const {
			return m_lastAccessTime;
		};
		FILETIME lastWriteTime() const {
			return m_lastWriteTime;
		};
		ULONGLONG filesize() const {
			return m_filesize;
		};
		const std::tstring& filename() const {
			return m_filename;
		};
	private:
		DWORD m_attribute;
		FILETIME m_createTime;
		FILETIME m_lastAccessTime;
		FILETIME m_lastWriteTime;
		ULONGLONG m_filesize;
		std::tstring m_filename;
	};

public:
	typedef std::list<CFindData> FindList;
	typedef std::list<std::tstring> FileList;

public:
	explicit CFileEnumrator(const TCHAR* target);
	explicit CFileEnumrator(const std::tstring& target);
	~CFileEnumrator();

private:
	void enumrate(const TCHAR* target);

public:
	FindList finddata_list() const {
		return m_list;
	};
	FileList filelist() const {
		FileList filenames;
		for(FindList::const_iterator it = m_list.begin(); it != m_list.end(); ++it) {
			filenames.push_back((*it).filename());
		}
		return filenames;
	};

private:
	FindList m_list;
};

}
}