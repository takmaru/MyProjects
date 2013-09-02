#pragma once

#include <wincrypt.h>

namespace MyLib {
namespace Crypto {

class CCryptoProvider {
public:
	CCryptoProvider() : m_cryptoProvider(NULL) {
	}
	explicit CCryptoProvider(HCRYPTPROV cryptoProvider) : m_cryptoProvider(cryptoProvider) {
	}
	~CCryptoProvider() {
		this->close();
	}
private:
	CCryptoProvider(const CCryptoProvider& copy);
	CCryptoProvider operator=(const CCryptoProvider& copy);
public:
	void attach(HCRYPTPROV cryptoProvider) {
		this->close();
		m_cryptoProvider = cryptoProvider;
	}
	HCRYPTPROV release() {
		HCRYPTPROV cryptProv = m_cryptoProvider;
		m_cryptoProvider = NULL;
		return cryptProv;
	}
	void close() {
		if(m_cryptoProvider != NULL) {
			::CryptReleaseContext(m_cryptoProvider, 0);
			m_cryptoProvider = NULL;
		}
	}
	operator HCRYPTPROV() const {
		return m_cryptoProvider;
	}
	operator HCRYPTPROV*() {
		return &m_cryptoProvider;
	}
	bool isOpen() const {
		return (m_cryptoProvider != NULL);
	}
private:
	HCRYPTPROV m_cryptoProvider;
};

}
}