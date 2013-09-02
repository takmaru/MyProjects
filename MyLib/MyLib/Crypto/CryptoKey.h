#pragma once

#include <wincrypt.h>

#include "../Data/BinaryData.h"

namespace MyLib {
namespace Crypto {

class CCryptoKey {
public:
	CCryptoKey() : m_cryptoKey(NULL) {
	}
	explicit CCryptoKey(HCRYPTKEY cryptoKey) : m_cryptoKey(cryptoKey) {
	}
	~CCryptoKey() {
		this->destroy();
	}
private:
	CCryptoKey(const CCryptoKey& copy);
	CCryptoKey operator=(const CCryptoKey& copy);
public:
	void attach(HCRYPTKEY cryptoKey) {
		this->destroy();
		m_cryptoKey = cryptoKey;
	}
	HCRYPTKEY release() {
		HCRYPTKEY cryptKey = m_cryptoKey;
		m_cryptoKey = NULL;
		return cryptKey;
	}
	void destroy() {
		if(m_cryptoKey != NULL) {
			::CryptDestroyKey(m_cryptoKey);
			m_cryptoKey = NULL;
		}
	}
	operator HCRYPTKEY() const {
		return m_cryptoKey;
	}
	operator HCRYPTKEY*() {
		return &m_cryptoKey;
	}
	bool isEnable() const {
		return (m_cryptoKey != NULL);
	}

public:
	void setAlgorithm(ALG_ID algid);
	MyLib::Data::BinaryData encrypto(const MyLib::Data::BinaryData& data);
	MyLib::Data::BinaryData decrypto(const MyLib::Data::BinaryData& encryptoData);

private:
	HCRYPTKEY m_cryptoKey;
};

}
}