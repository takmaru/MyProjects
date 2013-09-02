#include "stdafx.h"
#include "CryptoKey.h"

#include "../tstring/tstring.h"

void MyLib::Crypto::CCryptoKey::setAlgorithm(ALG_ID algid) {
    if(!::CryptSetKeyParam(m_cryptoKey, KP_ALGID, (const BYTE*)&algid, 0)) {
		std::tcout << _T("setAlgorithm CryptSetKeyParam err=") << ::GetLastError() << std::endl;
	}
}

MyLib::Data::BinaryData MyLib::Crypto::CCryptoKey::encrypto(const MyLib::Data::BinaryData& data) {
	DWORD encryptoDataLength = data.size();
    if(!::CryptEncrypt(m_cryptoKey, 0, TRUE, 0, NULL, &encryptoDataLength, data.size())) {
		std::tcout << _T("encrypto CryptEncrypt(GetLength) err=") << ::GetLastError() << std::endl;
		return MyLib::Data::BinaryData();
	}
	MyLib::Data::BinaryData encryptoData(encryptoDataLength, 0);
	std::copy(data.begin(), data.end(), encryptoData.begin());
    if(!::CryptEncrypt(m_cryptoKey, 0, TRUE, 0, &encryptoData[0], &encryptoDataLength, data.size())) {
		std::tcout << _T("encrypto CryptEncrypt err=") << ::GetLastError() << std::endl;
		return MyLib::Data::BinaryData();
	}
	return encryptoData;
}
MyLib::Data::BinaryData MyLib::Crypto::CCryptoKey::decrypto(const MyLib::Data::BinaryData& encryptoData) {
	MyLib::Data::BinaryData decryptoData(encryptoData);
	DWORD decryptoLength = decryptoData.size();
    if(!::CryptDecrypt(m_cryptoKey, 0, TRUE, 0, &decryptoData[0], &decryptoLength)) {
		std::tcout << _T("encrypto CryptDecrypt err=") << ::GetLastError() << std::endl;
		return MyLib::Data::BinaryData();
	}
	decryptoData.resize(decryptoLength);
	return decryptoData;
}
