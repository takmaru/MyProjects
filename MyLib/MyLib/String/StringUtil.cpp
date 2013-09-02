#include "stdafx.h"
#include "StringUtil.h"

#include <algorithm>

std::string MyLib::String::toMultibyte(const wchar_t* wc) {
	std::string strResult;
	int nLen = ::WideCharToMultiByte(CP_ACP, 0, wc, -1, NULL, 0, NULL, NULL);
	if(nLen == 0)	return strResult;
	char* mb = new char[nLen];
	if(::WideCharToMultiByte(CP_ACP, 0, wc, -1, mb, nLen, NULL, NULL) != 0)	strResult = mb;
	delete [] mb;
	return strResult;
}

std::wstring MyLib::String::toWideChar(const char* mb) {
	std::wstring strResult;
	int nLen = ::MultiByteToWideChar(CP_ACP, 0, mb, -1, NULL, 0);
	if(nLen == 0)	return strResult;
	wchar_t* pwc = new wchar_t[nLen];
	if(::MultiByteToWideChar(CP_ACP, 0, mb, -1, pwc, nLen) != 0)	strResult = pwc;
	delete [] pwc;
	return strResult;
}

std::tstring MyLib::String::fromOLECHAR(const LPCOLESTR str) {
	if(sizeof(TCHAR) == sizeof(str[0])) {
		return std::tstring(reinterpret_cast<LPCTSTR>(str));
	} else
	if(	(sizeof(TCHAR) == sizeof(wchar_t)) &&
		(sizeof(str[0]) == sizeof(char))	) {
		return std::tstring(reinterpret_cast<LPCTSTR>(MyLib::String::toWideChar(reinterpret_cast<const char*>(str)).c_str()));
	} else
	if(	(sizeof(TCHAR) == sizeof(char)) &&
		(sizeof(str[0]) == sizeof(wchar_t))	) {
		return std::tstring(reinterpret_cast<LPCTSTR>(MyLib::String::toMultibyte(reinterpret_cast<const wchar_t*>(str)).c_str()));
	}
	return std::tstring();
}

std::tstring MyLib::String::toHexStr(const unsigned char* pData, const unsigned int len,
	const bool big/*= false*/, const int unit/*= 1*/, const int block/*= 8*/, const int record/*= 16*/) {
	std::tstring strRet;
	// �p�����[�^�`�F�b�N
	if(	!pData ||
		(len == 0)	)	return strRet;

	const unsigned char* p = pData;
	int nCnt_Unit = 0;
	int nCnt_Block = 0;
	int nCnt_Record = 0;
	for(unsigned int i = 0; i < len; i++)
	{
		TCHAR tcHex[3] = {0};
		if(big)	_stprintf_s(tcHex, _T("%02X"), *p);
		else	_stprintf_s(tcHex, _T("%02x"), *p);
		strRet += tcHex;
		nCnt_Record++;
		nCnt_Block++;
		nCnt_Unit++;
		if(	(record > 0) &&
			(nCnt_Record >= record)	)
		{
			strRet += '\n';
			nCnt_Record = 0;
			nCnt_Block = 0;
			nCnt_Unit = 0;
		}
		else
		if(	(block > 0) &&
			(nCnt_Block >= block)	)
		{
			strRet += _T("  ");
			nCnt_Block = 0;
			nCnt_Unit = 0;
		}
		else
		if(	(unit > 0) &&
			(nCnt_Unit >= unit)	)
		{
			strRet += _T(" ");
			nCnt_Unit = 0;
		}
		p++;
	}
	strRet += '\n';
	return strRet;
}
std::vector<std::tstring> MyLib::String::toHexStrList(const unsigned char* pData, const unsigned int len,
	const bool big/*= false*/, const int unit/*= 1*/, const int block/*= 8*/, const int record/*= 16*/) {
	std::vector<std::tstring> vecRet;
	// �p�����[�^�`�F�b�N
	if(	!pData ||
		(len == 0)	)	return vecRet;

	const unsigned char* p = pData;
	std::tstring strRecord;
	int nCnt_Unit = 0;
	int nCnt_Block = 0;
	int nCnt_Record = 0;
	for(DWORD i = 0; i < len; i++)
	{
		TCHAR tcHex[3] = {0};
		if(big)	_stprintf_s(tcHex, _T("%02X"), *p);
		else	_stprintf_s(tcHex, _T("%02x"), *p);
		strRecord += tcHex;
		nCnt_Unit++;
		nCnt_Block++;
		nCnt_Record++;
		if(	(record > 0) &&
			(nCnt_Record >= record)	)
		{
			vecRet.push_back(strRecord);
			strRecord = _T("");
			nCnt_Record = 0;
			nCnt_Block = 0;
			nCnt_Unit = 0;
		}
		else
		if(	(block > 0) &&
			(nCnt_Block >= block)	)
		{
			strRecord += _T("  ");
			nCnt_Block = 0;
			nCnt_Unit = 0;
		}
		else
		if(	(unit > 0) &&
			(nCnt_Unit >= unit)	)
		{
			strRecord += ' ';
			nCnt_Unit = 0;
		}
		p++;
	}
	if(strRecord.size() > 0)	vecRet.push_back(strRecord);
	return vecRet;
}

std::tstring MyLib::String::trim(const std::tstring str, const E_TrimKind tk/*= Enm_TrimKind_Both*/) {
	TCHAR* pstr = _tcsdup(str.c_str());
	if(!pstr)	return str;

	int nLen = _tcslen(pstr);
	int nLeftPos = -1;
	int nRightPos = -1;
	bool bLeftTrimMode = (tk != Enm_TrimKind_Right);

	int i;
	for(i = 0; i < nLen; i++) {
		if(bLeftTrimMode) {
			// LeftTrim
#ifndef _UNICODE
			if(memcmp(pstr + i, _T("�@"), sizeof(_T("�@"))) == 0)	i++;
			else
#endif
			if(	!_istspace(pstr[i]) &&
				_istprint(pstr[i])	) {
				bLeftTrimMode = false;
				nLeftPos = i;
			}
		} else {
			// RightTrim
#ifndef _UNICODE
			if(memcmp(pstr + i, _T("�@"), sizeof(_T("�@"))) == 0) {
				nRightPos = i++;
			} else
#endif
			if(	_istspace(pstr[i]) ||
				!_istprint(pstr[i])	) {
				nRightPos = i;
			}
			else	nRightPos = -1;
		}
	}
	free(pstr);

	if(nLeftPos == -1)	nLeftPos = 0;
	if(nRightPos == -1)	nRightPos = nLen;

	return str.substr(nLeftPos, nRightPos - nLeftPos);
}

std::tstring MyLib::String::replace(const std::tstring str, const std::tstring src, const std::tstring dst, const bool bAll/*= true*/) {
	if(	(src.size() <= 0) ||
		(src.size() <= 0)	)	return str;

	std::tstring strRet = str;
	int nFindPos = 0;
	while(1) {
		int nRepPos = strRet.find(src, nFindPos);
		if(nRepPos == std::tstring::npos)	break;
		strRet.replace(nRepPos, src.size(), dst);
		if(!bAll)	break;
		nFindPos = nRepPos + dst.size();
	}

	return strRet;
}

std::vector<std::tstring> MyLib::String::split(const std::tstring& str, LPTSTR sep/*= NULL*/, LPTSTR enc/*= NULL*/) {
	std::vector<std::tstring> vecRet;

	// ��؂蕶������m��
	TCHAR tcDefault_SeparateChar[] = _T(",");
	PTCHAR pSepStr = tcDefault_SeparateChar;
	if(sep)	pSepStr = sep;
	// �͂���������m��
	TCHAR tcDefault_EnclosedChar[] = _T("\"");
	PTCHAR pEncStr = tcDefault_EnclosedChar;
	if(enc)	pEncStr = enc;
	bool bEnabledEncChar = (_tcslen(pEncStr) > 0);
			
	if(_tcslen(pSepStr) > 0) {
		// ��؂蕶���񂪎w�肳��Ă���
		// CSV���o���[�v
		int nPos = 0;
		int nStrLen = str.size();
		while(1) {
			std::tstring strFieldData;
			int nFindSep = str.find(pSepStr, nPos);
			int nFindEnc = std::tstring::npos;
			if(bEnabledEncChar) nFindEnc = str.find(pEncStr, nPos);
			// �͂��f�[�^�`�F�b�N�i��؂蕶�����͂������̕����悩�ǂ����j
			bool bCheckEncData =	(	(nFindEnc != std::tstring::npos) &&
										(	(nFindSep == std::tstring::npos) ||
											(nFindEnc < nFindSep)	)	);
			///////////////////////////////////
			// �͂��f�[�^�`�F�b�N����
			bool bFindEncData = false;	// �͂������f�[�^�����t���O
			if(bCheckEncData) {
				// �͂������̐擪���������
				// �͂��̏I�����������A�f�[�^�𔲂��o���B
				int nEncSearchStart = nFindEnc + 1;
				// �͂��̏I���������[�v�i�A�����Ă��Ȃ��͂������������j
				while(1) {
					int nEncSearchEnd = str.find(pEncStr, nEncSearchStart);
					// �͂��I�����Ȃ���΃��[�v�I��
					if(nEncSearchEnd == std::tstring::npos)	break;
					// �͂��I�����A�����Ă���΁A��������
					if(	((nEncSearchEnd + 1) < nStrLen) &&
						(str[nEncSearchEnd + 1] == pEncStr[0])	)
					{
						nEncSearchStart = nEncSearchEnd + 2;
						continue;
					}
					// �͂������f�[�^�A���̋�؂蕶���ʒu���Z�b�g
					std::tstring strRepSrc;
					strRepSrc += pEncStr;
					strRepSrc += pEncStr;
					strFieldData = MyLib::String::replace(str.substr(nEncSearchStart, nEncSearchEnd - nEncSearchStart), strRepSrc, std::tstring(pEncStr));
					nFindSep = str.find(pSepStr, nEncSearchEnd + 1);
					bFindEncData = true;
					break;
				}
			}
			// �͂��f�[�^�`�F�b�N���� ...end
			///////////////////////////////////
			if(!bFindEncData) {
				// �͂��f�[�^��������Ȃ������ꍇ�A
				// ���̋�؂蕶���܂ł��t�B�[���h�f�[�^
				if(nFindSep != std::tstring::npos)	strFieldData = str.substr(nPos, nFindSep - nPos);
				else							strFieldData = str.substr(nPos);
			}
			// �f�[�^�����X�g�ɒǉ�
			vecRet.push_back(strFieldData);

			// ���̋�؂蕶�����Ȃ���΃��[�v�I��
			if(nFindSep == std::tstring::npos)	break;
			// ���̌����J�n�ʒu���Z�b�g
			nPos = nFindSep + 1;
		}
	} else {
		// ��؂蕶���񂪃u�����N�̏ꍇ
		// �n���ꂽ��������Z�b�g���ĕԂ�
		vecRet.push_back(str);
	}
	return vecRet;
}

namespace {
	bool removeFromGuidStr(TCHAR c) {
		return (	(c == _T('-')) ||
					(c == _T('{')) ||
					(c == _T('}'))	);
	}
}
std::tstring MyLib::String::uid() {
	GUID guid = {0};
	if(::CoCreateGuid(&guid) != S_OK) {
		return std::tstring();
	}
	TCHAR guidStr[(sizeof(GUID) * 2) + 4 + 2 + 1] = {0};
	if(::StringFromGUID2(guid, guidStr, _countof(guidStr)) == 0) {
		return std::tstring();
	}
	std::tstring guidtstr(MyLib::String::fromOLECHAR(guidStr));
	guidtstr.erase(std::remove_if(guidtstr.begin(), guidtstr.end(), removeFromGuidStr), guidtstr.end());
	return guidtstr;
}

// ---------- End Of File --------------------------------------------------- //
