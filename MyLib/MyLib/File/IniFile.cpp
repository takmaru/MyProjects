#include "stdafx.h"
#include "IniFile.h"

#include <Windows.h>
#include <tchar.h>

#include "../Data/BinaryData.h"

namespace {
	static const int kDefaultBufferSize = 1024;
	static const int kExpandBufferSize = kDefaultBufferSize * 10;
}

std::tstring MyLib::File::getIniFileString(const std::tstring& iniPath, const std::tstring& section, const std::tstring& key,
										   const std::tstring& defaultValue) {

	std::vector<TCHAR> buffer(1024, _T('\0'));
	DWORD ret = ::GetPrivateProfileString(section.c_str(), key.c_str(), defaultValue.c_str(), &buffer[0], buffer.size(), iniPath.c_str());
	if(ret == (buffer.size() - 1)) {
		buffer.resize(kExpandBufferSize, _T('\0'));
		ret = ::GetPrivateProfileString(section.c_str(), key.c_str(), defaultValue.c_str(), &buffer[0], buffer.size(), iniPath.c_str());
	}
	return std::tstring(&buffer[0]);
}

int MyLib::File::getIniFileInteger(const std::tstring& iniPath, const std::tstring& section, const std::tstring& key,
								   int defaultValue) {
	return ::GetPrivateProfileInt(section.c_str(), key.c_str(), defaultValue, iniPath.c_str());
}
