#pragma once

#include <DHKExLib/DHKeyMaker.h>
#include <MyLib/Crypto/CryptoKey.h>

class CDHKeyServer {
public:
	CDHKeyServer();
	~CDHKeyServer();
public:
	void createPublickey();
	void setClientKey(const DHKEx::DHBlob& clientKey);

	DHKEx::DHBlob publicKey() const {
		return m_dhkeyMaker.publicKey();
	}
	DHKEx::DHBlob prime() const {
		return m_dhkeyMaker.prime();
	}
	DHKEx::DHBlob generator() const {
		return m_dhkeyMaker.generator();
	}

public:
	DHKEx::DHBlob encrypto(const DHKEx::DHBlob& data);
	DHKEx::DHBlob decrypto(const DHKEx::DHBlob& encryptoData);

private:
	DHKEx::CDHKeyMaker m_dhkeyMaker;
	MyLib::Crypto::CCryptoKey m_sessionKey;
};
