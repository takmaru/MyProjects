#pragma once

#include <Windows.h>
#include <tchar.h>
#include <string>
#include "../tstring/tstring.h"

namespace MyLib {
namespace Path {

std::tstring append(const std::tstring& src, const std::tstring& appendpath);
std::tstring append(const std::tstring& src, const TCHAR* appendpath);

std::tstring removeFilespec(const std::tstring& src);
std::tstring removeExtention(const std::tstring& src);
std::tstring renameExtention(const std::tstring& src, const std::tstring& extention);
std::tstring renameExtention(const std::tstring& src, const TCHAR* extention);

std::tstring filename(const std::tstring& path);
std::tstring filename(const TCHAR* path);
namespace A {
std::string filename(const std::string& path);
std::string filename(const char* path);
}
namespace W {
std::wstring filename(const std::wstring& path);
std::wstring filename(const wchar_t* path);
}

bool isExsist(const std::tstring& path);
bool isDirectory(const std::tstring& path);

}
}