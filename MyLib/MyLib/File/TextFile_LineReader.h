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
			// �t�@�C�����J���Ă��Ȃ���΁A�J��
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
				} else	RAISE_MYEXCEPTION("ReadFile�G���[");
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
					*m_pIdxReadBuf = CT_FILE::GetNull();	// \n��\0�ɒu������
					m_pIdxReadBuf++;						// �o�b�t�@�C���f�b�N�XAdd
					m_nReadBufLen -= sizeof(FileCharType);	// �o�b�t�@��Dec
				}
				bFindNextLine = true;
				m_bLFisFirstMode = false;
			} else {
				// �ȉ��̃��[�v�ŁA
				// ���s�����邩�ǂ��������߂�
				while(m_nReadBufLen > 0) {
					if(*m_pIdxReadBuf == CT_FILE::GetCR()) {
						*m_pIdxReadBuf = CT_FILE::GetNull();	// \r��\0�ɒu������
						m_pIdxReadBuf++;						// �o�b�t�@�C���f�b�N�XAdd
						m_nReadBufLen -= sizeof(FileCharType);	// �o�b�t�@��Dec
						if(m_nReadBufLen > 0) {
							if(*m_pIdxReadBuf == CT_FILE::GetLF()) {
								*m_pIdxReadBuf = CT_FILE::GetNull();	// \n��\0�ɒu������
								m_pIdxReadBuf++;						// �o�b�t�@�C���f�b�N�XAdd
								m_nReadBufLen -= sizeof(FileCharType);	// �o�b�t�@��Dec
							}
							bFindNextLine = true;
						} else {
							// \r�Ńo�b�t�@���I������ꍇ
							// ���񃊁[�h�̐擪��\n(LF)���m�F���郂�[�h�ֈڍs
							m_bLFisFirstMode = true;
						}
						break;
					}
					else if(*m_pIdxReadBuf == CT_FILE::GetLF()) {
						*m_pIdxReadBuf = CT_FILE::GetNull();	// \n��\0�ɒu������
						m_pIdxReadBuf++;						// �o�b�t�@�C���f�b�N�XAdd
						m_nReadBufLen -= sizeof(FileCharType);	// �o�b�t�@��Dec
						bFindNextLine = true;
						break;
					}

					m_pIdxReadBuf++;						// �o�b�t�@�C���f�b�N�XAdd
					m_nReadBufLen -= sizeof(FileCharType);	// �o�b�t�@��Dec
				}
			}

			// ���s�����������H	
			if(bFindNextLine) {
				// ���s�����������ꍇ
				// �L���b�V���Ƀf�[�^���Ȃ���΁A�o�b�t�@�̃|�C���^��Ԃ�
				if(m_nCacheLen == 0)	pResult = pStart;
				else {
					// �L���b�V���������
					// �o�b�t�@���L���b�V���փR�s�[����B
					_CopyBufferToCache(pStart, (nReadBufLenOnStart - m_nReadBufLen) / sizeof(FileCharType));
					// �L���b�V���̐擪�|�C���^��Ԃ��A�L���b�V���T�C�Y�� 0 ��
					pResult = &(*m_Cache.begin());
					m_nCacheLen = 0;
				}
			} else {
				// ���s��������Ȃ������ꍇ�i\r,\r\n,\n��������Ȃ��A�������́A�o�b�t�@�I����\r�̏ꍇ�j
				// �o�b�t�@���L���b�V���փR�s�[
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
			// �}���`�o�C�g�������烏�C�h�����֕ϊ�
			int nLineLen = ::MultiByteToWideChar(CP_ACP, 0, pBuf, -1, NULL, 0);
			if(nLineLen == 0) RAISE_MYEXCEPTION("MultiByteToWideChar �G���[");
			if((unsigned int)nLineLen > m_LineBuf.size())	m_LineBuf.assign(nLineLen, L'\0');
			::MultiByteToWideChar(CP_ACP, 0, pBuf, -1, &m_LineBuf.front(), m_LineBuf.size());
			return &m_LineBuf.front();
		}
		template<> char* CTextFile_LineReader<wchar_t, char>::_FileToUseCharType(wchar_t* pBuf) {
			// ���C�h��������}���`�o�C�g�����֕ϊ�
			int nLineLen = ::WideCharToMultiByte(CP_ACP, 0, pBuf, -1, NULL, 0, NULL, NULL);
			if(nLineLen == 0) RAISE_MYEXCEPTION("WideCharToMultiByte �G���[");
			if((unsigned int)nLineLen > m_LineBuf.size())	m_LineBuf.assign(nLineLen, '\0');
			::WideCharToMultiByte(CP_ACP, 0, pBuf, -1, &m_LineBuf.front(), m_LineBuf.size(), NULL, NULL);
			return &m_LineBuf.front();
		}
	};
};
// ---------- End Of File --------------------------------------------------- //
