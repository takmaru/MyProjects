#pragma once

#include "../tstring/tstring.h"
#include <vector>
#include <Windows.h>
#include <WTypesbase.h>

namespace MyLib {
namespace String {

	std::string  toMultibyte(const wchar_t* wc);
	std::wstring toWideChar(const char* mb);
	std::tstring fromOLECHAR(const LPCOLESTR str);

	std::tstring toHexStr(const unsigned char* pData, const unsigned int len,
		const bool big = false, const int unit = 1, const int block = 8, const int record = 16);
	std::vector<std::tstring> toHexStrList(const unsigned char* pData, const unsigned int len,
		const bool big = false, const int unit = 1, const int block = 8, const int record = 16);

	enum E_TrimKind {
		Enm_TrimKind_Both,
		Enm_TrimKind_Left,
		Enm_TrimKind_Right
	};
	std::tstring trim(const std::tstring str, const E_TrimKind tk = Enm_TrimKind_Both);

	std::tstring replace(const std::tstring str, const std::tstring src, const std::tstring dst, const bool isAll = true);

	std::vector<std::tstring> split(const std::tstring& str, TCHAR* sep = NULL, LPTSTR enc = NULL);

	// Forward Iterator が必要
	// FwIt が指すオブジェクトは、stringへ代入できる必要がある
	template<class FwIt>
	std::tstring toCSV(FwIt begin, FwIt end, TCHAR sep = ',', TCHAR enc = '"') {
		std::tstring strResult;

		if(sep == enc){sep = ',';	enc = '"';}

		FwIt it;
		for(it = begin; it != end; it++) {
			std::tstring strItem = (*it);

			if(strItem.find(sep, 0) != std::tstring::npos)
			{
				std::tstring rep = strItem;
				rep.replace(0, rep.size(), 2, enc);
				strItem = enc;	strItem += rep;	strItem += enc;
			}

			if(it != begin)	strResult += sep;
			strResult += strItem;
		}

		return strResult;
	};

	std::tstring uid();
}
}
// ---------- End Of File --------------------------------------------------- //
