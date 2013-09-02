#pragma once

#include "FileManager.h"
#include "../Exception/MyException.h"

#include <vector>
#include <xlocale>

namespace MyLib {
	namespace File {
		enum {
			MyLib_CharCode_SJIS,
			MyLib_CharCode_JIS,
			MyLib_CharCode_EUC,
			MyLib_CharCode_UTF16,
			MyLib_CharCode_UTF16BE,
			MyLib_CharCode_UTF8,
			MyLib_CharCode_UTF7
		};

		template<typename T>
		class CCharGetter {
		public:
			inline static T GetNull(){return '\0';}
			inline static T GetCR(){return '\r';}
			inline static T GetLF(){return '\n';}
		};
		template<> wchar_t CCharGetter<wchar_t>::GetNull(){return L'\0';}
		template<> wchar_t CCharGetter<wchar_t>::GetCR(){return L'\r';}
		template<> wchar_t CCharGetter<wchar_t>::GetLF(){return L'\n';}

		template<typename FileCharType, typename UseCharType/*, int FileCharCode = MyLib_CharCode_SJIS*/>
		class CTextFile_LineReader : CFileManager {
		private:
			typedef CCharGetter<FileCharType> CT_FILE;
			typedef CCharGetter<UseCharType> CT_USE;
		public:
			enum {
				Enm_CC_SJIS,
				Enm_CC_JIS,
				Enm_CC_EUC,
				Enm_CC_UTF16,
				Enm_CC_UTF16BE,
				Enm_CC_UTF8,
				Enm_CC_UTF7
			};
		public:
			explicit CTextFile_LineReader(LPCTSTR strFileName, int nReadFileLen = 4096, int nCharCode = Enm_CC_SJIS):
				CFileManager(strFileName, OM_Read), m_LineBuf(), m_CharCode(nCharCode),
				m_ReadBuf(nReadFileLen / sizeof(FileCharType)), m_pIdxReadBuf(&(*m_ReadBuf.begin())), m_nReadBufLen(0),
				m_bEOF(false), m_bLFisFirstMode(false),
				m_Cache(), m_nCacheLen(0){};
			virtual ~CTextFile_LineReader(){};
		public:
			bool ReadLine(std::basic_string<UseCharType>& strLine);
		private:
			FileCharType* _GetLine();
			FileCharType* _GetLineFromBuf();

			void _CopyBufferToCache(FileCharType* src, int size);

			UseCharType* _FileToUseCharType(FileCharType* pBuf);
		private:
			std::vector<UseCharType> m_LineBuf;
			int m_CharCode;

			std::vector<FileCharType> m_ReadBuf;
			FileCharType* m_pIdxReadBuf;
			int m_nReadBufLen;
			bool m_bEOF;
			bool m_bLFisFirstMode;

			std::vector<FileCharType> m_Cache;
			int m_nCacheLen;
		};

		template<typename FileCharType, typename UseCharType>
		bool CTextFile_LineReader<FileCharType, UseCharType>::
			ReadLine(std::basic_string<UseCharType>& strLine) {
			// ファイルを開いていなければ、開く
			if(!IsOpen())	Open();

			bool bResult = false;

			FileCharType* pLine = _GetLine();
			if(pLine != NULL) {
				strLine = _FileToUseCharType(pLine);
				bResult = true;
			}

			return bResult;
		}

		template<typename FileCharType, typename UseCharType>
		FileCharType* CTextFile_LineReader<FileCharType, UseCharType>::_GetLine() {
			if(m_bEOF)	return NULL;

			FileCharType* pResult = NULL;

			while((pResult = _GetLineFromBuf()) == NULL) {
				if(::ReadFile(GetHandle(), &m_ReadBuf.front(), m_ReadBuf.size() * sizeof(FileCharType), reinterpret_cast<DWORD*>(&m_nReadBufLen), NULL)) {
					if(m_nReadBufLen > 0)	m_pIdxReadBuf = &(*m_ReadBuf.begin());
					else {	// EOF
						m_bEOF = true;
						if(m_nCacheLen > 0) {
							FileCharType cNull = 0;
							_CopyBufferToCache(&cNull, 1);
							pResult = &(*m_Cache.begin());
						}
						break;
					}
				} else	RAISE_MYEXCEPTION("ReadFileエラー");
			}

			return pResult;
		}

		template<typename FileCharType, typename UseCharType>
		FileCharType* CTextFile_LineReader<FileCharType, UseCharType>::_GetLineFromBuf() {
			FileCharType* pResult = NULL;
			FileCharType* pStart = m_pIdxReadBuf;
			int nReadBufLenOnStart = m_nReadBufLen;

			bool bFindNextLine = false;
			if(m_bLFisFirstMode) {
				if(*m_pIdxReadBuf == CT_FILE::GetLF()) {
					*m_pIdxReadBuf = CT_FILE::GetNull();	// \nを\0に置き換え
					m_pIdxReadBuf++;						// バッファインデックスAdd
					m_nReadBufLen -= sizeof(FileCharType);	// バッファ長Dec
				}
				bFindNextLine = true;
				m_bLFisFirstMode = false;
			} else {
				// 以下のループで、
				// 改行があるかどうかを求める
				while(m_nReadBufLen > 0) {
					if(*m_pIdxReadBuf == CT_FILE::GetCR()) {
						*m_pIdxReadBuf = CT_FILE::GetNull();	// \rを\0に置き換え
						m_pIdxReadBuf++;						// バッファインデックスAdd
						m_nReadBufLen -= sizeof(FileCharType);	// バッファ長Dec
						if(m_nReadBufLen > 0) {
							if(*m_pIdxReadBuf == CT_FILE::GetLF()) {
								*m_pIdxReadBuf = CT_FILE::GetNull();	// \nを\0に置き換え
								m_pIdxReadBuf++;						// バッファインデックスAdd
								m_nReadBufLen -= sizeof(FileCharType);	// バッファ長Dec
							}
							bFindNextLine = true;
						} else {
							// \rでバッファが終了する場合
							// 次回リードの先頭が\n(LF)か確認するモードへ移行
							m_bLFisFirstMode = true;
						}
						break;
					}
					else if(*m_pIdxReadBuf == CT_FILE::GetLF()) {
						*m_pIdxReadBuf = CT_FILE::GetNull();	// \nを\0に置き換え
						m_pIdxReadBuf++;						// バッファインデックスAdd
						m_nReadBufLen -= sizeof(FileCharType);	// バッファ長Dec
						bFindNextLine = true;
						break;
					}

					m_pIdxReadBuf++;						// バッファインデックスAdd
					m_nReadBufLen -= sizeof(FileCharType);	// バッファ長Dec
				}
			}

			// 改行が見つかった？	
			if(bFindNextLine) {
				// 改行が見つかった場合
				// キャッシュにデータがなければ、バッファのポインタを返す
				if(m_nCacheLen == 0)	pResult = pStart;
				else {
					// キャッシュがあれば
					// バッファをキャッシュへコピーする。
					_CopyBufferToCache(pStart, (nReadBufLenOnStart - m_nReadBufLen) / sizeof(FileCharType));
					// キャッシュの先頭ポインタを返し、キャッシュサイズは 0 に
					pResult = &(*m_Cache.begin());
					m_nCacheLen = 0;
				}
			} else {
				// 改行が見つからなかった場合（\r,\r\n,\nが見つからない、もしくは、バッファ終了が\rの場合）
				// バッファをキャッシュへコピー
				_CopyBufferToCache(pStart, (nReadBufLenOnStart - m_nReadBufLen) / sizeof(FileCharType));
			}

			return pResult;
		}

		template<typename FileCharType, typename UseCharType>
		void CTextFile_LineReader<FileCharType, UseCharType>::
			_CopyBufferToCache(FileCharType* src, int size) {
			if(size <= 0)	return;
			int nShortageSize = size - (m_Cache.size() - m_nCacheLen);
			if(nShortageSize > 0)	m_Cache.resize(m_Cache.size() + nShortageSize);
			std::copy(src, src + size, m_Cache.begin() + m_nCacheLen);
			m_nCacheLen += size;
		}

		template<typename FileCharType, typename UseCharType>
		UseCharType* CTextFile_LineReader<FileCharType, UseCharType>::
			_FileToUseCharType(FileCharType* pBuf) {
			return pBuf;
		}
		template<> wchar_t* CTextFile_LineReader<char, wchar_t>::_FileToUseCharType(char* pBuf) {
			// マルチバイト文字からワイド文字へ変換
			int nLineLen = ::MultiByteToWideChar(CP_ACP, 0, pBuf, -1, NULL, 0);
			if(nLineLen == 0) RAISE_MYEXCEPTION("MultiByteToWideChar エラー");
			if((unsigned int)nLineLen > m_LineBuf.size())	m_LineBuf.assign(nLineLen, L'\0');
			::MultiByteToWideChar(CP_ACP, 0, pBuf, -1, &m_LineBuf.front(), m_LineBuf.size());
			return &m_LineBuf.front();
		}
		template<> char* CTextFile_LineReader<wchar_t, char>::_FileToUseCharType(wchar_t* pBuf) {
			// ワイド文字からマルチバイト文字へ変換
			int nLineLen = ::WideCharToMultiByte(CP_ACP, 0, pBuf, -1, NULL, 0, NULL, NULL);
			if(nLineLen == 0) RAISE_MYEXCEPTION("WideCharToMultiByte エラー");
			if((unsigned int)nLineLen > m_LineBuf.size())	m_LineBuf.assign(nLineLen, '\0');
			::WideCharToMultiByte(CP_ACP, 0, pBuf, -1, &m_LineBuf.front(), m_LineBuf.size(), NULL, NULL);
			return &m_LineBuf.front();
		}
	};
};
// ---------- End Of File --------------------------------------------------- //
