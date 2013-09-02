#include "stdafx.h"
#include "ApplicationInfo.h"

std::tstring MyLib::App::modulefilename() {
	TCHAR filename[1024] = {0};
	::GetModuleFileName(NULL, filename, _countof(filename));
	return std::tstring(filename);
}

std::tstring MyLib::App::modulepath() {
	std::tstring filename = MyLib::App::modulefilename();
	return filename.substr(0, filename.find_last_of(_T("\\")));
}
